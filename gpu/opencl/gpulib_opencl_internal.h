#ifndef _GPULIB_OPENCL_INTERNAL_H_
#define _GPULIB_OPENCL_INTERNAL_H


#include "gpulib_interface.h"

// Targeting OpenCL 1.2 and beyond
#define CL_TARGET_OPENCL_VERSION 120

// TODO: Find a better place for this
// https://www.khronos.org/registry/OpenCL/extensions/amd/cl_amd_device_attribute_query.txt
#define CL_DEVICE_BOARD_NAME_AMD 0x4038
#include "CL/cl.h"

namespace gpulib {
namespace opencl {

void cl_call(cl_int err);

class OpenCLDeviceMemory : public IDeviceMemory {

private:
  size_t _size;
  cl_context _context;
  cl_mem _mem;
  cl_command_queue _queue;

public:
  OpenCLDeviceMemory(cl_context context, cl_command_queue queue, size_t size);

  OpenCLDeviceMemory(cl_context context, cl_command_queue queue, void* host_ptr, size_t size);

  ~OpenCLDeviceMemory();

  void write(const void* src, size_t count_bytes);

  void write(const void* src, size_t offset, size_t count);

  void read(void* dst, size_t count_bytes);

  void read(void* dst, size_t offset, size_t count);


  void memset(uint32_t value);
  size_t size();

  cl_mem* get_cl_mem();
};


class OpenCLKernel : public IKernel {

private:
  cl_program _program;
  cl_context _context;
  cl_device_id _device;
  cl_kernel _kernel;
  cl_command_queue _queue;

  std::array<size_t, 3> _block_size = { { 1, 1, 1 } };
  std::array<size_t, 3> _grid_size = { { 1, 1, 1 } };

public:
  OpenCLKernel(cl_context context, cl_device_id device, cl_command_queue queue, cl_kernel kernel);
  OpenCLKernel(cl_context context, cl_device_id device, cl_command_queue queue, const std::string& src, const std::string entry);

  ~OpenCLKernel();

  void set_arg(int idx, const void* arg, size_t size);

  void set_arg(int idx, std::shared_ptr<IDeviceMemory> buffer);

  void set_block_size(std::array<size_t, 3> block_size);

  void set_grid_size(std::array<size_t, 3> grid_size);

  size_t get_max_threads_per_block();

  void call(bool blocking = true);

  std::array<size_t, 3> get_block_size()
  {
    return _block_size;
  }

  std::array<size_t, 3> get_grid_size()
  {
    return _grid_size;
  }
};

class OpenCLModule : public IModule {

private:
  cl_program _program;
  cl_context _ctx;
  cl_device_id _device;
  cl_command_queue _queue;

public:
  OpenCLModule(cl_program program, cl_context context, cl_device_id device, cl_command_queue queue);
  ~OpenCLModule();

  std::shared_ptr<IKernel> load_kernel(const std::string& entry);

};

class OpenCLDevice : public IDevice {

private:
  std::string _id;
  cl_device_id _device;
  cl_context _ctx;
  cl_command_queue _queue;

  size_t _global_mem_size = 0;
  size_t _max_buffer_size = 0;
  size_t _mp_count = 0;
  std::array<size_t, 3> _max_local_dim = { {1,1,1} };
  size_t _max_local_size = 1;

  std::string load_source(const std::string& file_path);

  void collect_device_info();

public:
  OpenCLDevice(cl_device_id id, const std::string& id_str);

  ~OpenCLDevice();

  std::shared_ptr<IDeviceMemory> malloc(size_t size);

  std::shared_ptr<IDeviceMemory> malloc_host(void* host_ptr, size_t size);

  std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& entry);

  std::shared_ptr<IKernel> load_kernel_from_file(const std::string& file_path, const std::string& entry);

  std::shared_ptr<IModule> load_module_from_source(const std::string& src);

  std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& module_name, const std::string& entry)
  {
    std::shared_ptr<IKernel> ptr(nullptr);
    return ptr;
  }

  size_t get_global_mem_size();

  std::array<size_t, 3> get_max_local_dim();

  size_t get_max_local_size();

  size_t get_mp_count();

  DeviceType get_type() { return DeviceType::OpenCL; }

  std::string get_unique_id()
  {
    return _id;
  }

  size_t max_buffer_size();
};

}
}

#endif