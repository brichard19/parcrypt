#include <iostream>

#include "gpulib_opencl.h"
#include "gpulib_opencl_internal.h"

gpulib::opencl::OpenCLKernel::OpenCLKernel(cl_context context, cl_device_id device, cl_command_queue queue, cl_kernel kernel)
  : _context(context)
  , _device(device)
  , _queue(queue)
  , _kernel(kernel)
{
}

gpulib::opencl::OpenCLKernel::OpenCLKernel(cl_context context, cl_device_id device, cl_command_queue queue, const std::string& src, const std::string entry) : _context(context), _device(device), _queue(queue)
{
  int err = 0;
  const char* src_ptr = src.c_str();
  size_t src_len = src.length();

  _program = clCreateProgramWithSource(_context, 1, &src_ptr, &src_len, &err);
  cl_call(err);

  err = clBuildProgram(_program, 0, nullptr, nullptr, nullptr, nullptr);

  int success = err;

  size_t log_size = 0;
  cl_call(clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size));

  std::vector<char> log(log_size);
  cl_call(clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr));

  std::string log_str(log.data(), log.size());

  if(success != CL_SUCCESS) {
    std::cout << log_str << std::endl;
    throw gpulib::Exception("OpenCL build error: " + std::to_string(success) + "\n" + log_str);
  }

  _kernel = clCreateKernel(_program, entry.c_str(), &err);
  cl_call(err);
}

gpulib::opencl::OpenCLKernel::~OpenCLKernel()
{
  clReleaseKernel(_kernel);
}

void gpulib::opencl::OpenCLKernel::set_arg(int idx, const void* arg, size_t size)
{
  cl_call(clSetKernelArg(_kernel, idx, size, arg));
}

void gpulib::opencl::OpenCLKernel::set_arg(int idx, std::shared_ptr<IDeviceMemory> buffer)
{
  OpenCLDeviceMemory* cl_buffer = static_cast<OpenCLDeviceMemory*>(buffer.get());
  cl_call(clSetKernelArg(_kernel, idx, sizeof(cl_mem), cl_buffer->get_cl_mem()));
}

void gpulib::opencl::OpenCLKernel::set_block_size(std::array<size_t, 3> block_size)
{
  _block_size = block_size;
}

void gpulib::opencl::OpenCLKernel::set_grid_size(std::array<size_t, 3> grid_size)
{
  _grid_size = grid_size;
}

void gpulib::opencl::OpenCLKernel::call(bool blocking)
{
  std::array<size_t, 3> global_size;
  global_size[0] = _grid_size[0] * _block_size[0];
  global_size[1] = _grid_size[1] * _block_size[1];
  global_size[2] = _grid_size[2] * _block_size[2];

  cl_call(clEnqueueNDRangeKernel(_queue, _kernel, 1, nullptr, global_size.data(), _block_size.data(), 0, nullptr, nullptr));
  if(blocking) {
    cl_call(clFinish(_queue));
  }
}

size_t gpulib::opencl::OpenCLKernel::get_max_threads_per_block()
{
  size_t max_size;
  cl_call(clGetKernelWorkGroupInfo(_kernel, _device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(max_size), &max_size, nullptr));

  return max_size;
}