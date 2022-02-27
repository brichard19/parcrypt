#include <iostream>

#include "gpulib_opencl.h"
#include "gpulib_opencl_internal.h"

std::string gpulib::opencl::OpenCLDevice::load_source(const std::string& file_path)
{
  std::ifstream f(file_path);
  if(!f.good()) {
    throw gpulib::Exception("OpenCL error: '" + file_path + "' not found");
  }

  std::stringstream buf;
  buf << f.rdbuf();

  return buf.str();
}

gpulib::opencl::OpenCLDevice::OpenCLDevice(cl_device_id id, const std::string& id_str) : _id(id_str), _device(id)
{
  cl_int err;
  _ctx = clCreateContext(0, 1, &_device, nullptr, nullptr, &err);
  cl_call(err);

  _queue = clCreateCommandQueue(_ctx, _device, 0, &err);
  cl_call(err);

  collect_device_info();
}

gpulib::opencl::OpenCLDevice::~OpenCLDevice()
{
  clReleaseCommandQueue(_queue);
  clReleaseContext(_ctx);
}

std::shared_ptr<gpulib::IDeviceMemory> gpulib::opencl::OpenCLDevice::malloc(size_t size)
{
  std::shared_ptr<IDeviceMemory> ptr;
  ptr.reset(new OpenCLDeviceMemory(_ctx, _queue, size));
  return ptr;
}

std::shared_ptr<gpulib::IDeviceMemory> gpulib::opencl::OpenCLDevice::malloc_host(void* host_ptr, size_t size)
{
  std::shared_ptr<IDeviceMemory> ptr;
  ptr.reset(new OpenCLDeviceMemory(_ctx, _queue, host_ptr, size));
  return ptr;
}

std::shared_ptr<gpulib::IKernel> gpulib::opencl::OpenCLDevice::load_kernel_from_source(const std::string& src, const std::string& entry)
{
  std::shared_ptr<IKernel> ptr;
  ptr.reset(new OpenCLKernel(_ctx, _device, _queue, src, entry));
  return ptr;
}

std::shared_ptr<gpulib::IKernel> gpulib::opencl::OpenCLDevice::load_kernel_from_file(const std::string& file_path, const std::string& entry)
{
  std::string src = load_source(file_path);

  std::shared_ptr<IKernel> ptr;
  ptr.reset(new OpenCLKernel(_ctx, _device, _queue, src, entry));
  return ptr;
}

std::shared_ptr<gpulib::IModule> gpulib::opencl::OpenCLDevice::load_module_from_source(const std::string& src)
{
  int err = 0;
  const char* src_ptr = src.c_str();
  size_t src_len = src.length();

  cl_program program = clCreateProgramWithSource(_ctx, 1, &src_ptr, &src_len, &err);
  cl_call(err);

  err = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);

  int success = err;

  size_t log_size = 0;
  cl_call(clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size));

  std::vector<char> log(log_size);
  cl_call(clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr));

  std::string log_str(log.data(), log.size());

  if(success != CL_SUCCESS) {
    std::cout << log_str << std::endl;
    throw gpulib::Exception("OpenCL build error: " + std::to_string(success) + "\n" + log_str);
  }

  std::shared_ptr<IModule> ptr;
  ptr.reset(new OpenCLModule(program, _ctx, _device, _queue));

  return ptr;
}

size_t gpulib::opencl::OpenCLDevice::get_global_mem_size()
{
  return _global_mem_size;
}

size_t gpulib::opencl::OpenCLDevice::get_mp_count()
{
  return _mp_count;
}

void gpulib::opencl::OpenCLDevice::collect_device_info()
{
  cl_ulong mem_size;
  cl_call(clGetDeviceInfo(_device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, nullptr));

  _global_mem_size = static_cast<size_t>(mem_size);

  cl_uint mp_count;
  cl_call(clGetDeviceInfo(_device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(mp_count), &mp_count, nullptr));
  _mp_count = mp_count;

  cl_call(clGetDeviceInfo(_device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(_max_local_dim), _max_local_dim.data(), nullptr));

  cl_ulong max_buffer_size;
  cl_call(clGetDeviceInfo(_device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_buffer_size), &max_buffer_size, nullptr));
  _max_buffer_size = max_buffer_size;
}

std::array<size_t, 3> gpulib::opencl::OpenCLDevice::get_max_local_dim()
{
  return _max_local_dim;
}

size_t gpulib::opencl::OpenCLDevice::get_max_local_size()
{
  return _max_local_size;
}

size_t gpulib::opencl::OpenCLDevice::max_buffer_size()
{
  return _max_buffer_size;
}