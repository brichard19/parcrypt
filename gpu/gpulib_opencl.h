#ifndef _GPULIB_OPENCL_H_
#define _GPULIB_OPENCL_H_

#include <vector>

#include "gpulib_interface.h"

namespace gpulib {

namespace opencl {
bool is_opencl_supported();

std::vector<DeviceInfo> get_opencl_devices();

}
}

#endif