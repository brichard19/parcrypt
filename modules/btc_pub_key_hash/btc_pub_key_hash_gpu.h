#ifndef _KEY_SEARCH_DEVICE_H
#define _KEY_SEARCH_DEVICE_H

#include <thread>
#include <deque>

#include "gpulib.h"
#include "gpu_config.h"
#include "secp256k1.h"
#include "settings_cache.h"
#include "util.h"
#include "worker_device.h"



class BTCPubKeyHashGPU : public WorkerDevice {

private:
  // GPU stuff
  bool _kernels_loaded = false;

  std::string _device_id;

  gpulib::Device _device;
  gpulib::Module _module;
  gpulib::Kernel _kernel_hash;
  gpulib::Tuner _tuner_hash;

  gpulib::Kernel _kernel_increment_keys;
  gpulib::Kernel _kernel_increment_keys_double;
  gpulib::Kernel _kernel_hash_keys;
  gpulib::Kernel _kernel_init_pubkeys;

  gpulib::DeviceMemory _dev_x;
  gpulib::DeviceMemory _dev_y;
  gpulib::DeviceMemory _dev_gx;
  gpulib::DeviceMemory _dev_gy;
  gpulib::DeviceMemory _dev_inv;
  gpulib::DeviceMemory _dev_target_hash;
  gpulib::DeviceMemory _dev_inc_x;
  gpulib::DeviceMemory _dev_inc_y;

  gpulib::DeviceMemory _dev_result_flag;
  gpulib::DeviceMemory _dev_result_idx;
 
  bool _compressed = true;

  size_t _mem_usage = 128 * 1024 * 1024;
  std::string _target;
  int32_t _num_points = 0;
  uint32_t _target_hash[5] = { 0 };

  secp256k1::uint256 _key_start;
  secp256k1::uint256 _key_end;
  secp256k1::uint256 _key_current;
  secp256k1::uint256 _private_key;

  util::Timer _timer;
  uint64_t _iterations = 0;
  uint64_t _prev_iterations = 0;
  double _prev_time = 0.0;
  bool _running = false;
  bool _done = false;
  bool _found = false;

  void setup_kernels();
  void allocate_memory();

  void init_gpu_points();

  void set_args();

  void prepare_target_hash(uint32_t* target_hash);

  std::thread* _thread = nullptr;

  static void worker_thread(BTCPubKeyHashGPU* d);

  double _key_rate = 0.0;

  void step();

  secp256k1::uint256 get_next_key();

  void tune_kernel(gpulib::Kernel k, const std::string& name);
  void tune_kernels();
  bool are_kernels_tuned();
  void load_tuning();

  SettingsCache& _settings = SettingsCache::instance();

  std::deque<double> _timings;

  void set_mem_usage_pct(double usage);

  void set_mem_usage_bytes(size_t usage);

public:
  BTCPubKeyHashGPU(gpulib::Device& device);
  virtual ~BTCPubKeyHashGPU();

  void set_mem_usage(const MemUsage& usage);

  void init(secp256k1::uint256 key_start, secp256k1::uint256 key_end, const std::string& target, bool compressed = true);

  bool get_result(secp256k1::uint256& private_key);

  void sanity_check();

  bool done();

  uint64_t keys_per_step();

  float progress();

  void start();

  void stop();

  double key_rate();
};

#endif
