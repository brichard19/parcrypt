#include <iostream>
#include <string>


#include "gpu/gpulib.h"
#include "modules/btc_pub_key_hash/btc_pub_key_hash_gpu.h"
#include "platform.h"
#include "util.h"

bool btc_pub_key_hash_gpu_find_key_compressed(gpulib::Device& gpu)
{
  bool success = true;

  std::string addr = "1LHtnpd8nU5VHEMkG2TMYYNUjjLc992bps";
  secp256k1::uint256 start(1);
  secp256k1::uint256 end((uint64_t)0xfffffffff);

  BTCPubKeyHashGPU* worker = new BTCPubKeyHashGPU(gpu);

  worker->init(start, end, addr, true);

  worker->start();

  while(!worker->done()) {
    platform::sleep(1.5);
    std::cout << util::format(worker->progress() * 100.0, 1) << "%" << std::endl;
  }

  secp256k1::uint256 private_key;
  if(!worker->get_result(private_key)) {
    std::cout << "Error: No private key found" << std::endl;
    success = false;
  } else {
    std::cout << "Found private key:" << std::endl;
    std::cout << private_key.to_string() << std::endl;
  }

  delete worker;

  return success;
}

bool btc_pub_key_hash_gpu_find_key_uncompressed(gpulib::Device& gpu)
{
  bool success = true;

  //std::string addr = "1LHtnpd8nU5VHEMkG2TMYYNUjjLc992bps";
  std::string addr = "13FDSfmW4nYdde4oBAS71C1mhE43KX1GxT";
  secp256k1::uint256 start(1);
  secp256k1::uint256 end((uint64_t)0xfffffffff);

  BTCPubKeyHashGPU* worker = new BTCPubKeyHashGPU(gpu);

  worker->init(start, end, addr, false);

  worker->start();

  while(!worker->done()) {
    platform::sleep(1.5);
    std::cout << util::format(worker->progress() * 100.0, 1) << "%" << std::endl;
  }

  secp256k1::uint256 private_key;
  if(!worker->get_result(private_key)) {
    std::cout << "Error: No private key found" << std::endl;
    success = false;
  } else {
    std::cout << "Found private key:" << std::endl;
    std::cout << private_key.to_string() << std::endl;
  }

  delete worker;

  return success;
}

int main(int argc, char** argv)
{
  auto devices = gpulib::get_devices(); 

  for(auto& d : devices) {
    if(d.type == gpulib::DeviceType::OpenCL) {
      gpulib::Device device = gpulib::get_device(d);
      std::cout << d.device_name << std::endl;
      if(!btc_pub_key_hash_gpu_find_key_compressed(device)) {
        std::cout << "FAIL" << std::endl;
      } else {
        std::cout << "PASS" << std::endl;
      }
      if(!btc_pub_key_hash_gpu_find_key_uncompressed(device)) {
        std::cout << "FAIL" << std::endl;
      } else {
        std::cout << "PASS" << std::endl;
      }
    }
  }

  return 0;
}