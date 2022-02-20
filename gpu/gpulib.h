#ifndef _GPULIB_H
#define _GPULIB_H

#include "gpulib_interface.h"

namespace gpulib {

std::vector<DeviceInfo> get_devices();
Device get_device(const DeviceInfo& info);

}

#endif
