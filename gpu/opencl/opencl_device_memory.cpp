#include "gpulib_opencl.h"
#include "gpulib_opencl_internal.h"

gpulib::opencl::OpenCLDeviceMemory::OpenCLDeviceMemory(cl_context context, cl_command_queue queue, size_t size) : _context(context), _queue(queue), _size(size)
{
  int err = CL_SUCCESS;
  _mem = clCreateBuffer(_context, 0, size, nullptr, &err);
  cl_call(err);
}

gpulib::opencl::OpenCLDeviceMemory::OpenCLDeviceMemory(cl_context context, cl_command_queue queue, void* host_ptr, size_t size) : _context(context), _queue(queue), _size(size)
{
  int err = CL_SUCCESS;
  _mem = clCreateBuffer(_context, CL_MEM_USE_HOST_PTR, size, host_ptr, &err);
  cl_call(err);
}

gpulib::opencl::OpenCLDeviceMemory::~OpenCLDeviceMemory()
{
  clReleaseMemObject(_mem);
}

void gpulib::opencl::OpenCLDeviceMemory::write(const void* src, size_t count_bytes)
{
  write(src, 0, count_bytes);
}

void gpulib::opencl::OpenCLDeviceMemory::write(const void* src, size_t offset, size_t count_bytes)
{
  cl_call(clEnqueueWriteBuffer(_queue, _mem, CL_TRUE, offset, count_bytes, src, 0, nullptr, nullptr));
  cl_call(clFinish(_queue));
}

void gpulib::opencl::OpenCLDeviceMemory::read(void* dst, size_t count_bytes)
{
  read(dst, 0, count_bytes);
}

void gpulib::opencl::OpenCLDeviceMemory::read(void* dst, size_t offset, size_t count_bytes)
{
  cl_call(clEnqueueReadBuffer(_queue, _mem, CL_TRUE, offset, count_bytes, dst, 0, nullptr, nullptr));
  cl_call(clFinish(_queue));
}

void gpulib::opencl::OpenCLDeviceMemory::memset(uint32_t value)
{
  cl_call(clEnqueueFillBuffer(_queue, _mem, &value, sizeof(value), 0, _size, 0, nullptr, nullptr));
  cl_call(clFinish(_queue));
}

size_t gpulib::opencl::OpenCLDeviceMemory::size()
{
  return _size;
}

cl_mem* gpulib::opencl::OpenCLDeviceMemory::get_cl_mem()
{
  return &_mem;
}