#ifndef _GPU_CONFIG_H
#define _GPU_CONFIG_H

#include "gpulib.h"

// Class representing desired memory usage. Can be a percentage
// of available memory or a number of bytes.
struct MemUsage {
  enum class MemUsageType {
    Percent = 1,
    Bytes = 2,
  };

  MemUsageType type;
  double usage;

  MemUsage()
  {
    type = MemUsageType::Bytes;
    usage = 128.0 * 1024 * 1024;
  }

  MemUsage(MemUsageType type, double value)
  {
    switch(type) {
    case MemUsageType::Percent:
      if(value <= 0.0 || value > 1.0) {
        throw std::runtime_error("Mem usage percent must be between 0.0 and 1.0");
      }
      break;
    case MemUsageType::Bytes:
      break;
    default:
      throw std::runtime_error("Invalid mem usage type");
    }

    this->usage = value;
    this->type = type;
  }
};

class GPUConfig {
public:

  int api;
  int device_id;
  MemUsage mem_usage = MemUsage(MemUsage::MemUsageType::Bytes, 128.0 * 1024 * 1024);
  gpulib::DeviceInfo device_info;
};

#endif