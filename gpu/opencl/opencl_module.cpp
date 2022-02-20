#include "gpulib_opencl.h"
#include "gpulib_opencl_internal.h"



gpulib::opencl::OpenCLModule::OpenCLModule(cl_program program, cl_context context, cl_device_id device, cl_command_queue queue)
  : _program(program)
  , _ctx(context)
  , _device(device)
  , _queue(queue)
{
}

gpulib::opencl::OpenCLModule::~OpenCLModule()
{
  clReleaseProgram(_program);
}

std::shared_ptr<gpulib::IKernel> gpulib::opencl::OpenCLModule::load_kernel(const std::string& entry)
{
  int err = 0;
  cl_kernel kernel = clCreateKernel(_program, entry.c_str(), &err);

  std::shared_ptr<IKernel> ptr;
  ptr.reset(new gpulib::opencl::OpenCLKernel(_ctx, _device, _queue, kernel));
  return ptr;
}
