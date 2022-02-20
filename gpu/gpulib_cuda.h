#ifndef _GPULIB_CUDA_H_
#define _GPULIB_CUDA_H_

#include "gpulib_interface.h"

#include <cuda.h>

#include <vector>
#include <map>

namespace gpulib {
namespace cuda {

class CUDADeviceMemory : public IDeviceMemory {

private:
    CUdevice _device;
    CUdeviceptr _mem = 0;
    size_t _size;

public:
    CUDADeviceMemory(CUdevice device, size_t size);
    ~CUDADeviceMemory();

    void read(void* dst, size_t count);
    void read(void* dst, size_t offset, size_t count);
    void write(const void* src, size_t count);
    void write(const void* src, size_t offset, size_t count);
    void memset(uint32_t value);

    size_t size();

    CUdeviceptr get_mem()
    {
        return _mem;
    }
};

class CUDAKernel : public IKernel {

private:
    std::map<int, std::vector<uint8_t>> _args;

    CUfunction _function;

    std::array<size_t, 3> _grid_size = { {1, 1, 1} };
    std::array<size_t, 3> _block_size = { {1, 1, 1} };

public:

    CUDAKernel(CUfunction function);
    ~CUDAKernel();

    void set_arg(int arg_idx, const void* arg, size_t size);

    void set_arg(int idx, std::shared_ptr<IDeviceMemory> buffer);

    void set_block_size(std::array<size_t, 3> size);

    void set_grid_size(std::array<size_t, 3> grid_size);

    void call();
};

class CUDADevice : public IDevice {

private:
    int _device_id = -1;
    CUdevice _device;
    CUcontext _ctx;
    size_t _mp_count = 0;

    std::map<std::string, CUmodule> _modules;

public:
    CUDADevice(int device_id);
    ~CUDADevice();

    std::shared_ptr<IDeviceMemory> malloc(size_t size);

    std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& entry);

    std::shared_ptr<IKernel> load_kernel_from_file(const std::string& file_path, const std::string& entry);

    std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& module_name, const std::string& entry);

    size_t get_global_mem_size();

    std::array<size_t, 3> get_max_local_dim();

    size_t get_max_local_size();

    size_t get_mp_count();

    int get_type()
    {
        return gpulib::CUDA;
    }
};

std::vector<DeviceInfo> get_cuda_devices();

}
}
#endif