#include "gpulib_opencl.h"
#include "gpulib_opencl_internal.h"

namespace {

std::string to_lower(const std::string& s)
{
  std::string s2 = "";

  for(auto c : s) {
    s2 += tolower(c);
  }

  return s2;
}

bool ends_with(const std::string s, const std::string suffix)
{
  if(s.length() < suffix.length()) {
    return false;
  }

  return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool starts_with(const std::string s, const std::string prefix)
{
  if(s.length() < prefix.length()) {
    return false;
  }

  return s.find(prefix) == 0;
}

// Remove any unwanted artifacts from the name
std::string sanitize_name(const std::string& name)
{
  // Remove trademark text
  const std::string tm = "(tm)";
  std::string new_name = name;

  size_t pos = to_lower(new_name).find(tm);
  if(pos != std::string::npos) {
    new_name = new_name.substr(0, pos) + new_name.substr(pos + tm.length());
  }

  // Remove "Series" at the end of the name
  const std::string series = " series";
  if(ends_with(to_lower(new_name), series)) {
    new_name = new_name.substr(0, new_name.length() - series.length());
  }

  // Remove vendor name from beginning
  const std::string amd = "amd ";
  if(starts_with(to_lower(new_name), amd)) {
    new_name = new_name.substr(amd.length());
  }

  return new_name;
}

} // namespace

std::vector<gpulib::DeviceInfo> gpulib::opencl::get_opencl_devices()
{
  std::vector<gpulib::DeviceInfo> device_list;

  if(is_opencl_supported() == false) {
    return device_list;
  }


  cl_uint platform_count = 0;

  cl_call(clGetPlatformIDs(0, NULL, &platform_count));

  if(platform_count == 0) {
    return device_list;
  }

  std::vector<cl_platform_id> platforms(platform_count);

  cl_call(clGetPlatformIDs(platform_count, platforms.data(), nullptr));

  int platform_idx = 0;
  for(auto platform : platforms) {

    cl_uint device_count = 0;
    cl_call(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &device_count));

    if(device_count == 0) {
      continue;
    }

    std::vector<cl_device_id> devices(device_count);
    cl_call(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_count, devices.data(), nullptr));

    int device_idx = 0;
    for(auto device : devices) {
      char buf[256] = { 0 };

      gpulib::DeviceInfo info;
      size_t size;

      // Get device name
      // First see if its an AMD device
      if(clGetDeviceInfo(device, CL_DEVICE_BOARD_NAME_AMD, sizeof(buf), buf, &size) != CL_SUCCESS) {
        cl_call(clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buf), buf, &size));
      }

      info.device_name = sanitize_name(std::string(buf));

      int cores = 0;
      cl_call(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cores), &cores, nullptr));

      info.compute_units = cores;

      cl_ulong mem;
      cl_call(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem), &mem, nullptr));

      char vendor[128] = { 0 };
      cl_call(clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr));
      info.vendor_name = std::string(vendor);

      /*
      if(info.vendor.find("nvidia") != std::string::npos || info.vendor.find("NVIDIA") != std::string::npos) {
          int bus_id = -1;
          int slot_id = -1;
          clCall(clGetDeviceInfo(devices[j], CL_DEVICE_PCI_BUS_ID_NV, sizeof(bus_id), &bus_id, NULL));
          clCall(clGetDeviceInfo(devices[j], CL_DEVICE_PCI_SLOT_ID_NV, sizeof(slot_id), &slot_id, NULL));
          info.bus_id = bus_id;
          info.slot_id = slot_id;
      }*/

      info.type = DeviceType::OpenCL;
      info.mem_size = (uint64_t)mem;
      info.opencl_device_id = (uint64_t)device;
      info.opencl_platform_id = (uint64_t)platform;
      info.opencl_device_idx = device_idx;
      info.opencl_platform_idx = platform_idx;
      info.device_id_str = "cl_" + std::to_string(info.opencl_platform_idx) + "_" + std::to_string(info.opencl_device_idx);
      device_list.push_back(info);

      device_idx++;
    }
    platform_idx++;
  }

  return device_list;
}