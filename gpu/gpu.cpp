#include "gpulib_interface.h"

#include <vector>

#include "gpulib_opencl.h"
#include "opencl/gpulib_opencl_internal.h"

namespace gpulib {

std::vector<DeviceInfo> get_devices()
{
  std::vector<DeviceInfo> devices;

  std::vector<DeviceInfo> opencl_devices = opencl::get_opencl_devices();

  devices.insert(devices.end(), opencl_devices.begin(), opencl_devices.end());


#ifdef USE_CUDA
  std::vector<DeviceInfo> cuda_devices = cuda::get_cuda_devices();

  devices.insert(devices.end(), cuda_devices.begin(), cuda_devices.end());
#endif
  return devices;
}


std::vector<DeviceInfo> get_opencl_devices()
{
  return opencl::get_opencl_devices();
}


#ifdef USE_CUDA
inline std::vector<DeviceInfo> get_cuda_devices()
{
  return cuda::get_cuda_devices();
}
#endif

Device get_device(const DeviceInfo& info)
{
  std::shared_ptr<IDevice> ptr(nullptr);

  switch(info.type) {

  case DeviceType::OpenCL:
    ptr.reset(new opencl::OpenCLDevice(reinterpret_cast<cl_device_id>(info.opencl_device_id), info.device_id_str));
    break;

#ifdef USE_CUDA
  case device_type::CUDA:
    ptr.reset(new cuda::CUDADevice(info.cuda_device_id));
    break;
#endif
  default:
    throw gpulib::Exception("Invalid compute platform");
  }

  return ptr;
}
}
