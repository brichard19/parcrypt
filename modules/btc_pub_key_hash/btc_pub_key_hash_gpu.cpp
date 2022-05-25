
#include <numeric>
#include <stdexcept>

#include "fmt/format.h"
#include "btc_pub_key_hash_gpu.h"
#include "address_util.h"
#include "util.h"
#include "logger.h"

extern char _btc_pub_key_hash_cl_source[];

BTCPubKeyHashGPU::BTCPubKeyHashGPU(gpulib::Device& device)
  : _device(device)
{
}

BTCPubKeyHashGPU::~BTCPubKeyHashGPU()
{
}

void BTCPubKeyHashGPU::set_mem_usage(const MemUsage& usage)
{
  if(usage.type == MemUsage::MemUsageType::Percent) {
    set_mem_usage_pct(usage.usage);
  } else {
    set_mem_usage_bytes(static_cast<size_t>(usage.usage));
  }
}

void BTCPubKeyHashGPU::set_mem_usage_pct(double usage)
{
  if(usage <= 0.0f || usage > 1.0f) {
    throw std::invalid_argument("Invalid value for mem usage");
  }

  size_t gpu_mem_size = _device->get_global_mem_size();

  set_mem_usage_bytes(static_cast<size_t>(gpu_mem_size * usage));
}

void BTCPubKeyHashGPU::set_mem_usage_bytes(size_t usage)
{
  _mem_usage = usage;
}

void BTCPubKeyHashGPU::init(secp256k1::uint256 key_start, secp256k1::uint256 key_end, const std::string& target, bool compressed)
{
  _compressed = compressed;

  if(_device->get_type() != gpulib::DeviceType::OpenCL) {
    throw std::invalid_argument("Only OpenCL devices are supported");
  }

  if(key_start < 1) {
    throw std::invalid_argument("Start key must be at least 1");
  }

  if(key_start > key_end) {
    throw std::invalid_argument("Start key is higher than end key");
  }

  if(!Address::verify_address(target)) {
    throw std::invalid_argument("Not a valid Bitcoin address");
  }

  _device_id = _device->get_unique_id();

  _done = false;
  _found = false;

  if(key_start < 1) {
    _key_start = 1;
  } else {
    _key_start = key_start;
  }

  _key_start = key_start;
  _key_end = key_end;
  _key_current = _key_start;
  _target = target;

  Base58::to_hash160(_target, _target_hash);
  prepare_target_hash(_target_hash);

  // Calculate the number of points based on memory usage:
  // X, Y and the chaining buffer. Use the mem_usage parameter and
  // device limits to determine how much memory to allocate

  size_t max_buffer_size = _device->max_buffer_size();
  size_t max_points = max_buffer_size / sizeof(secp256k1::uint256_t);
  size_t bytes_per_point = sizeof(secp256k1::uint256_t) * 3;

  _num_points = (uint32_t)(((double)_mem_usage) / (double)bytes_per_point);

  if(_num_points > max_points) {
    _num_points = static_cast<uint32_t>(max_points);
  }

  if(!_kernels_loaded) {
    setup_kernels();
  }

  allocate_memory();

  init_gpu_points();

  set_args();

  if(!are_kernels_tuned()) {
    tune_kernels();
  } else {
    load_tuning();
  }

  // Re-initialize the GPU points after optimization
  init_gpu_points();
}

// For more efficient cracking, do some pre-processing by removing
// subtracting the RIPEMD160 IV from the target hash so that we can
// skip adding them on the GPU
void BTCPubKeyHashGPU::prepare_target_hash(uint32_t* target_hash)
{
  const uint32_t RIPEMD160_IV[5] = {
    0x67452301,
    0xefcdab89,
    0x98badcfe,
    0x10325476,
    0xc3d2e1f0
  };

  target_hash[0] = util::endian(target_hash[0]) - RIPEMD160_IV[1];
  target_hash[1] = util::endian(target_hash[1]) - RIPEMD160_IV[2];
  target_hash[2] = util::endian(target_hash[2]) - RIPEMD160_IV[3];
  target_hash[3] = util::endian(target_hash[3]) - RIPEMD160_IV[4];
  target_hash[4] = util::endian(target_hash[4]) - RIPEMD160_IV[0];
}

void BTCPubKeyHashGPU::setup_kernels()
{
  // Determine the different grid sizes we should try
  size_t gpu_mp_count = _device->get_mp_count();

  // Load the kernels
  _module = _device->load_module_from_source(_btc_pub_key_hash_cl_source);

  _kernel_init_pubkeys = _module->load_kernel("init_public_keys");
  _kernel_init_pubkeys->set_grid_size({ gpu_mp_count * 8, 1, 1 });
  _kernel_init_pubkeys->set_block_size({ 64, 1, 1 });

  _kernel_increment_keys = _module->load_kernel("increment_keys");

  _kernel_increment_keys_double = _module->load_kernel("increment_keys_with_double");

  if(_compressed) {
    _kernel_hash_keys = _module->load_kernel("hash_public_keys_compressed");
  } else {
    _kernel_hash_keys = _module->load_kernel("hash_public_keys_uncompressed");
  }
  _kernels_loaded = true;
}

bool BTCPubKeyHashGPU::are_kernels_tuned()
{
  std::vector<std::string> kernel_names;
  kernel_names.push_back("increment_keys");
  kernel_names.push_back("increment_keys_double");

  if(_compressed) {
    kernel_names.push_back("hash_public_keys_compressed");
  } else {
    kernel_names.push_back("hash_public_keys_uncompressed");
  }

  bool tuned = true;
  for(auto n : kernel_names) {
    tuned &= _settings.exists(_device_id + n + "_block_size");
    tuned &= _settings.exists(_device_id + n + "_grid_size");
  }

  return tuned;
}

void BTCPubKeyHashGPU::load_tuning()
{
  size_t block_size = util::parse_int(_settings.get(_device_id + "increment_keys_block_size"));
  size_t grid_size = util::parse_int(_settings.get(_device_id + "increment_keys_grid_size"));

  _kernel_increment_keys->set_block_size({ block_size, 1, 1 });
  _kernel_increment_keys->set_grid_size({ grid_size, 1, 1 });

  block_size = util::parse_int(_settings.get(_device_id + "increment_keys_double_block_size"));
  grid_size = util::parse_int(_settings.get(_device_id + "increment_keys_double_grid_size"));

  _kernel_increment_keys_double->set_block_size({ block_size, 1, 1 });
  _kernel_increment_keys_double->set_grid_size({ grid_size, 1, 1 });

  if(_compressed) {
    block_size = util::parse_int(_settings.get(_device_id + "hash_public_keys_compressed_block_size"));
    grid_size = util::parse_int(_settings.get(_device_id + "hash_public_keys_compressed_grid_size"));
  } else {
    block_size = util::parse_int(_settings.get(_device_id + "hash_public_keys_uncompressed_block_size"));
    grid_size = util::parse_int(_settings.get(_device_id + "hash_public_keys_uncompressed_grid_size"));
  }
  _kernel_hash_keys->set_block_size({ block_size, 1, 1 });
  _kernel_hash_keys->set_grid_size({ grid_size, 1, 1 });
}

// TODO: Update gpulib so we can get the kernel name from the
// Kernel object
void BTCPubKeyHashGPU::tune_kernel(gpulib::Kernel kernel, const std::string& name)
{
  // Determine the different grid sizes we should try
  size_t gpu_mp_count = _device->get_mp_count();

  // Use grid sizes between 1 and 16 blocks per multi-processor
  std::vector<std::array<size_t, 3>> grid_sizes;
  for(size_t i = 1; i <= 8; i *= 2) {
    grid_sizes.push_back({ gpu_mp_count * i, 1, 1 });
  }

  // Use block sizes between 32 and 256 threads
  std::vector<std::array<size_t, 3>> block_sizes;
  for(size_t i = 32; i <= 256; i *= 2) {
    block_sizes.push_back({ i, 1, 1 });
  }

  std::string block_id = _device_id + name + "_block_size";
  std::string grid_id = _device_id + name + "_grid_size";

  // Increment keys kernel
  gpulib::Tuner tuner_iterate = gpulib::Tuner(kernel, block_sizes, grid_sizes);
  do {
    tuner_iterate.run();
  } while(tuner_iterate.tuning());

  size_t block_size = kernel->get_block_size()[0];
  size_t grid_size = kernel->get_grid_size()[0];
  _settings.set(block_id, std::to_string(block_size));
  _settings.set(grid_id, std::to_string(grid_size));
}

void BTCPubKeyHashGPU::tune_kernels()
{
  tune_kernel(_kernel_increment_keys, "increment_keys");
  tune_kernel(_kernel_increment_keys_double, "increment_keys_double");

  if(_compressed) {
    tune_kernel(_kernel_hash_keys, "hash_public_keys_compressed");
  } else {
    tune_kernel(_kernel_hash_keys, "hash_public_keys_uncompressed");
  }
}

void BTCPubKeyHashGPU::allocate_memory()
{
  // Allocate memory
  _dev_x = _device->malloc(_num_points * sizeof(secp256k1::uint256_t));
  _dev_y = _device->malloc(_num_points * sizeof(secp256k1::uint256_t));
  _dev_inv = _device->malloc(_num_points * sizeof(secp256k1::uint256_t));
  _dev_target_hash = _device->malloc(5 * sizeof(uint32_t));

  _dev_result_flag = _device->malloc(sizeof(int));
  _dev_result_idx = _device->malloc(sizeof(int));

  _dev_gx = _device->malloc(256 * sizeof(secp256k1::uint256_t));
  _dev_gy = _device->malloc(256 * sizeof(secp256k1::uint256_t));

  _dev_inc_x = _device->malloc(sizeof(secp256k1::uint256_t));
  _dev_inc_y = _device->malloc(sizeof(secp256k1::uint256_t));

  _dev_result_flag->memset(0);
  _dev_result_idx->memset(0);
}

void BTCPubKeyHashGPU::init_gpu_points()
{
  const size_t count = 256;
  std::vector<secp256k1::uint256_t> gx;
  std::vector<secp256k1::uint256_t> gy;
  secp256k1::ecpoint g = secp256k1::G();

  for(int i = 0; i < 256; i++) {
    gx.push_back(g.x);
    gy.push_back(g.y);
    g = secp256k1::doublePoint(g);
  }

  _dev_gx->write(gx.data(), count * sizeof(secp256k1::uint256_t));
  _dev_gy->write(gy.data(), count * sizeof(secp256k1::uint256_t));

  // Generate key pairs for k, k+1, k+2 ... k + <total points in parallel - 1>
  gpulib::DeviceMemory dev_key_start = _device->malloc(sizeof(secp256k1::uint256_t));
  secp256k1::uint256_t tmp = _key_start;
  dev_key_start->write(&tmp, sizeof(tmp));

  // Initialize x, y to point-at-infinity
  _dev_x->memset(0xffffffff);
  _dev_y->memset(0xffffffff);

  for(int i = 0; i < 256; i++) {

    _kernel_init_pubkeys->set_arg(0, _num_points);
    _kernel_init_pubkeys->set_arg(1, i);
    _kernel_init_pubkeys->set_arg(2, dev_key_start);
    _kernel_init_pubkeys->set_arg(3, _dev_inv);
    _kernel_init_pubkeys->set_arg(4, _dev_gx);
    _kernel_init_pubkeys->set_arg(5, _dev_gy);
    _kernel_init_pubkeys->set_arg(6, _dev_x);
    _kernel_init_pubkeys->set_arg(7, _dev_y);

    _kernel_init_pubkeys->call();
  }

  sanity_check();

  // Copy hash to device
  _dev_target_hash->write(_target_hash, sizeof(_target_hash));

  // Set the incrementor point
  secp256k1::uint256 inc_k = secp256k1::uint256(_num_points);
  secp256k1::ecpoint inc_p = secp256k1::multiplyPoint(inc_k, secp256k1::G());

  secp256k1::uint256_t x = inc_p.x;
  secp256k1::uint256_t y = inc_p.y;

  _dev_inc_x->write(&x, sizeof(x));
  _dev_inc_y->write(&y, sizeof(y));
}

void BTCPubKeyHashGPU::set_args()
{
  _kernel_hash_keys->set_arg(0, _dev_x);
  _kernel_hash_keys->set_arg(1, _dev_y);
  _kernel_hash_keys->set_arg(2, _dev_target_hash);
  _kernel_hash_keys->set_arg(3, _num_points);
  _kernel_hash_keys->set_arg(4, _dev_result_flag);
  _kernel_hash_keys->set_arg(5, _dev_result_idx);

  _kernel_increment_keys_double->set_arg(0, _num_points);
  _kernel_increment_keys_double->set_arg(1, 0); // Compressed point
  _kernel_increment_keys_double->set_arg(2, _dev_inv);
  _kernel_increment_keys_double->set_arg(3, _dev_x);
  _kernel_increment_keys_double->set_arg(4, _dev_y);
  _kernel_increment_keys_double->set_arg(5, _dev_inc_x);
  _kernel_increment_keys_double->set_arg(6, _dev_inc_y);

  _kernel_increment_keys->set_arg(0, _num_points);
  _kernel_increment_keys->set_arg(1, 0); // Compressed point
  _kernel_increment_keys->set_arg(2, _dev_inv);
  _kernel_increment_keys->set_arg(3, _dev_x);
  _kernel_increment_keys->set_arg(4, _dev_y);
  _kernel_increment_keys->set_arg(5, _dev_inc_x);
  _kernel_increment_keys->set_arg(6, _dev_inc_y);
}

void BTCPubKeyHashGPU::step()
{
  _kernel_hash_keys->call(false);
  if(_key_current <= _num_points) {
    _kernel_increment_keys_double->call(true);
  } else {
    _kernel_increment_keys->call(true);
  }

  // Check for results
  unsigned int result_idx = 0;

  int flag = 0;
  _dev_result_flag->read(&flag, sizeof(flag));
  if(flag) {
    _found = true;
    _done = true;
    _dev_result_idx->read(&result_idx, sizeof(result_idx));

    // Calculate the private key
    _private_key = _key_current + result_idx;
  }

  _key_current = _key_current + _num_points;

  if(_key_current > _key_end) {
    _done = true;
  }
}

void BTCPubKeyHashGPU::sanity_check()
{
  // Check the first 100 points
  const size_t count = 100;

  std::vector<secp256k1::uint256_t> x_words(count);
  std::vector<secp256k1::uint256_t> y_words(count);

  _dev_x->read(x_words.data(), count * sizeof(secp256k1::uint256_t));
  _dev_y->read(y_words.data(), count * sizeof(secp256k1::uint256_t));

  for(int i = 0; i < count; i++) {

    secp256k1::ecpoint tmp = secp256k1::multiplyPoint(_key_start + i, secp256k1::G());
    secp256k1::uint256 x = secp256k1::uint256(x_words[i].v);
    secp256k1::uint256 y = secp256k1::uint256(y_words[i].v);

    secp256k1::ecpoint p(x, y);

    if(!p.exists() || p != tmp) {
      std::string err = fmt::format("Sanity check failed for GPU {0}: there might be an error in the code", _device_id);
      throw std::runtime_error(err);
    }
  }
}

secp256k1::uint256 BTCPubKeyHashGPU::get_next_key()
{
  return _key_current;
}

bool BTCPubKeyHashGPU::done()
{
  return _done;
}

bool BTCPubKeyHashGPU::get_result(secp256k1::uint256& private_key)
{
  private_key = _private_key;

  return _found;
}

uint64_t BTCPubKeyHashGPU::keys_per_step()
{
  return _num_points;
}

float BTCPubKeyHashGPU::progress()
{
  double key_space_size = (_key_end - _key_start + 1).to_double();

  double current = (_key_current - _key_start).to_double();

  return static_cast<float>(current / key_space_size);
}

void BTCPubKeyHashGPU::start()
{
  _running = true;
  _thread = new std::thread(BTCPubKeyHashGPU::worker_thread, this);
}

void BTCPubKeyHashGPU::stop()
{
  _running = false;

  if(_thread == nullptr) {
    return;
  }

  _thread->join();
  delete _thread;
  _thread = nullptr;
}

void BTCPubKeyHashGPU::worker_thread(BTCPubKeyHashGPU* d)
{

  d->_timer.start();
  d->_iterations = 0;
  while(d->_running) {

    d->step();
    d->_iterations++;

    if(d->done()) {
      d->_running = false;
      break;
    }
  }
}

double BTCPubKeyHashGPU::key_rate()
{
  double current_time = _timer.elapsed();
  uint64_t current_iterations = _iterations;

  double dt = current_time - _prev_time;

  double rate = static_cast<double>((current_iterations - _prev_iterations) * keys_per_step()) / dt;

  _prev_iterations = _iterations;
  _prev_time = current_time;
  return rate;
}
