#include "gpulib_opencl.h"

#include "gpulib_opencl_internal.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


// Declares a pointer type to the function and also declares a pointer of that type
#define CL_FUNCTION_PTR(ret, func, ...) typedef ret (CL_API_CALL* func ## _t)(__VA_ARGS__);\
func ## _t func ## _ptr = nullptr;

namespace {

// TODO: Add functions as we need them
CL_FUNCTION_PTR(cl_int, clBuildProgram, cl_program, cl_uint, const cl_device_id*, const char*, void (CL_CALLBACK*)(cl_program, void*), void*)
CL_FUNCTION_PTR(cl_int, clCompileProgram, cl_program, cl_uint, const cl_device_id*, const char*, cl_uint, const cl_program*, const char**, void (CL_CALLBACK*)(cl_program, void*), void*)
CL_FUNCTION_PTR(cl_mem, clCreateBuffer, cl_context, cl_mem_flags, size_t, void*, cl_int*)
CL_FUNCTION_PTR(cl_command_queue, clCreateCommandQueue, cl_context, cl_device_id, cl_command_queue_properties, cl_int*)
CL_FUNCTION_PTR(cl_context, clCreateContext, const cl_context_properties*, cl_uint, const cl_device_id*, void (CL_CALLBACK*)(const char*, const void*, size_t, void*), void*, cl_int*)
CL_FUNCTION_PTR(cl_kernel, clCreateKernel, cl_program, const char*, cl_int*)
CL_FUNCTION_PTR(cl_program, clCreateProgramWithBinary, cl_context, cl_uint, const cl_device_id*, const size_t*, const unsigned char**, cl_int*, cl_int*)
CL_FUNCTION_PTR(cl_program, clCreateProgramWithSource, cl_context, cl_uint, const char**, const size_t*, cl_int*)
CL_FUNCTION_PTR(cl_int, clEnqueueFillBuffer, cl_command_queue, cl_mem, const void*, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*)
CL_FUNCTION_PTR(cl_int, clEnqueueCopyBuffer, cl_command_queue, cl_mem, cl_mem, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*)
CL_FUNCTION_PTR(void*, clEnqueueMapBuffer, cl_command_queue, cl_mem, cl_bool, cl_map_flags, size_t, size_t, cl_uint, const cl_event*, cl_event*, cl_int*)
CL_FUNCTION_PTR(cl_int, clEnqueueNDRangeKernel, cl_command_queue, cl_kernel, cl_uint, const size_t*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*)
CL_FUNCTION_PTR(cl_int, clEnqueueReadBuffer, cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*)
CL_FUNCTION_PTR(cl_int, clEnqueueWriteBuffer, cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*)
CL_FUNCTION_PTR(cl_int, clFinish, cl_command_queue)
CL_FUNCTION_PTR(cl_int, clFlush, cl_command_queue)
CL_FUNCTION_PTR(cl_int, clGetDeviceIDs, cl_platform_id, cl_device_type, cl_uint, cl_device_id*, cl_uint*)
CL_FUNCTION_PTR(cl_int, clGetDeviceInfo, cl_device_id, cl_device_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clGetEventInfo, cl_event, cl_event_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clGetKernelWorkGroupInfo, cl_kernel, cl_device_id, cl_kernel_work_group_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clGetPlatformIDs, cl_uint, cl_platform_id*, cl_uint*)
CL_FUNCTION_PTR(cl_int, clGetPlatformInfo, cl_platform_id, cl_platform_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clGetProgramBuildInfo, cl_program, cl_device_id, cl_program_build_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clGetProgramInfo, cl_program, cl_program_info, size_t, void*, size_t*)
CL_FUNCTION_PTR(cl_int, clReleaseCommandQueue, cl_command_queue)
CL_FUNCTION_PTR(cl_int, clReleaseContext, cl_context)
CL_FUNCTION_PTR(cl_int, clReleaseEvent, cl_event)
CL_FUNCTION_PTR(cl_int, clReleaseKernel, cl_kernel)
CL_FUNCTION_PTR(cl_int, clReleaseMemObject, cl_mem)
CL_FUNCTION_PTR(cl_int, clReleaseProgram, cl_program)
CL_FUNCTION_PTR(cl_int, clSetKernelArg, cl_kernel, cl_uint, size_t, const void*)
CL_FUNCTION_PTR(cl_int, clWaitForEvents, cl_uint, const cl_event*)

#ifdef _WIN32
#define CL_LOAD_FUNCTION(library, func) func ## _ptr = reinterpret_cast<func ## _t>(GetProcAddress(library, #func))
#else
#define CL_LOAD_FUNCTION(library, func) func ## _ptr = reinterpret_cast<func ## _t>(dlsym(library, #func))
#endif

bool initialize_opencl()
{
#ifdef _WIN32
  HINSTANCE library = LoadLibrary("OpenCL.dll");
#else
  std::vector<const char*> paths = {
    "libOpenCL.so",
    "libOpenCL.so.1"
  };
  void* library = NULL;
  
  for(auto& path : paths) {
    library = dlopen(path, RTLD_LAZY);
    if(library) {
      break;
    }
  }
#endif

  if(library == NULL) {
    return false;
  }

  CL_LOAD_FUNCTION(library, clBuildProgram);
  CL_LOAD_FUNCTION(library, clCompileProgram);
  CL_LOAD_FUNCTION(library, clCreateBuffer);
  CL_LOAD_FUNCTION(library, clCreateCommandQueue);
  CL_LOAD_FUNCTION(library, clCreateContext);
  CL_LOAD_FUNCTION(library, clCreateKernel);
  CL_LOAD_FUNCTION(library, clCreateProgramWithBinary);
  CL_LOAD_FUNCTION(library, clCreateProgramWithSource);
  CL_LOAD_FUNCTION(library, clEnqueueFillBuffer);
  CL_LOAD_FUNCTION(library, clEnqueueCopyBuffer);
  CL_LOAD_FUNCTION(library, clEnqueueMapBuffer);
  CL_LOAD_FUNCTION(library, clEnqueueNDRangeKernel);
  CL_LOAD_FUNCTION(library, clEnqueueReadBuffer);
  CL_LOAD_FUNCTION(library, clEnqueueWriteBuffer);
  CL_LOAD_FUNCTION(library, clFinish);
  CL_LOAD_FUNCTION(library, clFlush);
  CL_LOAD_FUNCTION(library, clGetDeviceIDs);
  CL_LOAD_FUNCTION(library, clGetDeviceInfo);
  CL_LOAD_FUNCTION(library, clGetEventInfo);
  CL_LOAD_FUNCTION(library, clGetKernelWorkGroupInfo);
  CL_LOAD_FUNCTION(library, clGetPlatformIDs);
  CL_LOAD_FUNCTION(library, clGetPlatformInfo);
  CL_LOAD_FUNCTION(library, clGetProgramBuildInfo);
  CL_LOAD_FUNCTION(library, clGetProgramInfo);
  CL_LOAD_FUNCTION(library, clReleaseCommandQueue);
  CL_LOAD_FUNCTION(library, clReleaseContext);
  CL_LOAD_FUNCTION(library, clReleaseEvent);
  CL_LOAD_FUNCTION(library, clReleaseKernel);
  CL_LOAD_FUNCTION(library, clReleaseMemObject);
  CL_LOAD_FUNCTION(library, clReleaseProgram);
  CL_LOAD_FUNCTION(library, clSetKernelArg);
  CL_LOAD_FUNCTION(library, clWaitForEvents);

  return true;
}


} // namespace

cl_int clBuildProgram(
  cl_program program,
  cl_uint num_devices,
  const cl_device_id* device_list,
  const char* options,
  void (CL_CALLBACK* pfn_notify)(cl_program program, void* user_data),
  void* user_data)
{
  return clBuildProgram_ptr(program, num_devices, device_list, options, pfn_notify, user_data);
}

cl_int clGetDeviceIDs(
  cl_platform_id platform,
  cl_device_type device_type,
  cl_uint num_entries,
  cl_device_id* devices,
  cl_uint* num_devices)
{
  return clGetDeviceIDs_ptr(platform, device_type, num_entries, devices, num_devices);
}

cl_int clReleaseCommandQueue(
  cl_command_queue command_queue)
{
  return clReleaseCommandQueue_ptr(command_queue);
}

cl_command_queue clCreateCommandQueue(
  cl_context context,
  cl_device_id device,
  cl_command_queue_properties properties,
  cl_int* errcode_ret)
{
  return clCreateCommandQueue_ptr(context, device, properties, errcode_ret);
}

cl_int clReleaseContext(
  cl_context context)
{
  return clReleaseContext_ptr(context);
}

cl_context clCreateContext(
  const cl_context_properties* properties,
  cl_uint num_devices,
  const cl_device_id* devices,
  void (CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info, size_t cb, void* user_data),
  void* user_data,
  cl_int* errcode_ret)
{
  return clCreateContext_ptr(properties, num_devices, devices, pfn_notify, user_data, errcode_ret);
}

cl_int clGetProgramBuildInfo(
  cl_program program,
  cl_device_id device,
  cl_program_build_info param_name,
  size_t param_value_size,
  void* param_value,
  size_t* param_value_size_ret)
{
  return clGetProgramBuildInfo_ptr(program, device, param_name, param_value_size, param_value, param_value_size_ret);
}

cl_program clCreateProgramWithSource(
  cl_context context,
  cl_uint count,
  const char** strings,
  const size_t* lengths,
  cl_int* errcode_ret)
{
  return clCreateProgramWithSource_ptr(context, count, strings, lengths, errcode_ret);
}

cl_int clReleaseProgram(
  cl_program program)
{
  return clReleaseProgram_ptr(program);
}

cl_kernel clCreateKernel(
  cl_program program,
  const char* kernel_name,
  cl_int* errcode_ret)
{
  return clCreateKernel_ptr(program, kernel_name, errcode_ret);
}

cl_int clReleaseKernel(
  cl_kernel kernel)
{
  return clReleaseKernel_ptr(kernel);
}

cl_int clEnqueueNDRangeKernel(
  cl_command_queue command_queue,
  cl_kernel kernel,
  cl_uint work_dim,
  const size_t* global_work_offset,
  const size_t* global_work_size,
  const size_t* local_work_size,
  cl_uint num_events_in_wait_list,
  const cl_event* event_wait_list,
  cl_event* event)
{
  return clEnqueueNDRangeKernel_ptr(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
}

cl_int clFinish(
  cl_command_queue command_queue)
{
  return clFinish_ptr(command_queue);
}

cl_int clGetKernelWorkGroupInfo(
  cl_kernel kernel,
  cl_device_id device,
  cl_kernel_work_group_info param_name,
  size_t param_value_size,
  void* param_value,
  size_t* param_value_size_ret)
{
  return clGetKernelWorkGroupInfo_ptr(kernel, device, param_name, param_value_size, param_value, param_value_size_ret);
}

cl_int clSetKernelArg(
  cl_kernel kernel,
  cl_uint arg_index,
  size_t arg_size,
  const void* arg_value)
{
  return clSetKernelArg_ptr(kernel, arg_index, arg_size, arg_value);
}

cl_mem clCreateBuffer(
  cl_context context,
  cl_mem_flags flags,
  size_t size,
  void* host_ptr,
  cl_int* errcode_ret)
{
  return clCreateBuffer_ptr(context, flags, size, host_ptr, errcode_ret);
}

cl_int clEnqueueReadBuffer(
  cl_command_queue command_queue,
  cl_mem buffer,
  cl_bool blocking_read,
  size_t offset,
  size_t size,
  void* ptr,
  cl_uint num_events_in_wait_list,
  const cl_event* event_wait_list,
  cl_event* event)
{
  return clEnqueueReadBuffer_ptr(command_queue, buffer, blocking_read, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
}

cl_int clEnqueueWriteBuffer(
  cl_command_queue command_queue,
  cl_mem buffer,
  cl_bool blocking_write,
  size_t offset,
  size_t size,
  const void* ptr,
  cl_uint num_events_in_wait_list,
  const cl_event* event_wait_list,
  cl_event* event)
{
  return clEnqueueWriteBuffer_ptr(command_queue, buffer, blocking_write, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
}

cl_int clReleaseMemObject(
  cl_mem memobj)
{
  return clReleaseMemObject_ptr(memobj);
}

cl_int clEnqueueFillBuffer(
  cl_command_queue command_queue,
  cl_mem buffer,
  const void* pattern,
  size_t pattern_size,
  size_t offset,
  size_t size,
  cl_uint num_events_in_wait_list,
  const cl_event* event_wait_list,
  cl_event* event)
{
  return clEnqueueFillBuffer_ptr(command_queue, buffer, pattern, pattern_size, offset, size, num_events_in_wait_list, event_wait_list, event);
}

cl_int clGetPlatformIDs(
  cl_uint num_entries,
  cl_platform_id* platforms,
  cl_uint* num_platforms)
{
  return clGetPlatformIDs_ptr(num_entries, platforms, num_platforms);
}

cl_int clGetDeviceInfo(
  cl_device_id device,
  cl_device_info param_name,
  size_t param_value_size,
  void* param_value,
  size_t* param_value_size_ret)
{
  return clGetDeviceInfo_ptr(device, param_name, param_value_size, param_value, param_value_size_ret);
}




bool gpulib::opencl::is_opencl_supported()
{
  static bool initialized = false;
  static bool supported = false;

  if(initialized) {
    return supported;
  }

  supported = initialize_opencl();
  initialized = true;

  return supported;
}