#ifndef _GPU_CONFIG_H
#define _GPU_CONFIG_H

#include "gpulib.h"

class GPUConfig {
public:
  int api;
  int device_id;
  float mem_usage = 0.8f;
  gpulib::DeviceInfo device_info;
};

#endif