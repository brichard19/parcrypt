#include "gpulib_interface.h"
#include "gpulib_opencl_internal.h"

#include <string>

void gpulib::opencl::cl_call(cl_int err)
{
  if(err != CL_SUCCESS) {
    throw gpulib::Exception("OpenCL error code " + std::to_string(err));
  }
}