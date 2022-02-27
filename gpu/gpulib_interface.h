#ifndef _GPULIB_INTERFACE_H
#define _GPULIB_INTERFACE_H

#include <array>
#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>

namespace gpulib {

enum class DeviceType {
    OpenCL,
    CUDA
};


struct DeviceInfo {
    DeviceType type;
    uint64_t mem_size;
    int compute_units;

    std::string vendor_name;
    std::string device_name;

    std::string device_id_str;

    // OpenCL device info
    uint64_t opencl_device_id;
    uint64_t opencl_platform_id;
    int opencl_platform_idx;
    int opencl_device_idx;

    // CUDA device info
    int cuda_device_id;
};



class IDeviceMemory {

public:
    virtual void write(const void* src, size_t count) = 0;

    virtual void write(const void* src, size_t dst_offset, size_t count) = 0;

    virtual void read(void* dst, size_t count) = 0;

    virtual void read(void* dst, size_t offset, size_t count) = 0;

    virtual void memset(uint32_t value) = 0;

    // Helper templates
    template<typename T> void write(std::vector<T>& src)
    {
        write(static_cast<const void*>(src.data()), src.size() * sizeof(T));
    }

    template<typename T> void read(std::vector<T>& dst)
    {
        read(dst.data(), dst.size() * sizeof(T));
    }

    virtual size_t size() = 0;
};


class IKernel {

public:

    virtual void set_arg(int idx, const void* arg, size_t size) = 0;

    virtual void set_arg(int idx, std::shared_ptr<IDeviceMemory> buffer) = 0;

    template<typename T> void set_arg(int idx, T arg)
    {
        set_arg(idx, &arg, sizeof(T));
    }

    virtual void call(bool blocking = true) = 0;

    virtual void set_block_size(std::array<size_t, 3> local_size) = 0;

    virtual void set_grid_size(std::array<size_t, 3> grid_size) = 0;

    virtual size_t get_max_threads_per_block() = 0;

    virtual std::array<size_t, 3> get_grid_size() = 0;

    virtual std::array<size_t, 3> get_block_size() = 0;
};

class IModule {

public:

    virtual std::shared_ptr<IKernel> load_kernel(const std::string& entry) = 0;
};

class IDevice {

public:

    virtual std::shared_ptr<IDeviceMemory> malloc(size_t size) = 0;

    virtual std::shared_ptr<IDeviceMemory> malloc_host(void* host_ptr, size_t size) = 0;

    template<typename T> std::shared_ptr<IDeviceMemory> malloc(size_t size)
    {
        return malloc(size * sizeof(T));
    }

    virtual std::shared_ptr<IKernel> load_kernel_from_file(const std::string& file_path, const std::string& entry) = 0;

    virtual std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& entry) = 0;

    virtual std::shared_ptr<IKernel> load_kernel_from_source(const std::string& src, const std::string& module_name, const std::string& entry) = 0;

    virtual std::shared_ptr<IModule> load_module_from_source(const std::string& src) = 0;

    virtual size_t get_global_mem_size() = 0;

    virtual std::array<size_t, 3> get_max_local_dim() = 0;

    virtual size_t get_max_local_size() = 0;

    virtual size_t get_mp_count() = 0;

    virtual DeviceType get_type() = 0;

    virtual std::string get_unique_id() = 0;

    virtual size_t max_buffer_size() = 0;
};


typedef std::shared_ptr<IDevice> Device;
typedef std::shared_ptr<IKernel> Kernel;
typedef std::shared_ptr<IDeviceMemory> DeviceMemory;
typedef std::shared_ptr<IModule> Module;

class Exception {

private:
    std::string _msg;

public:
    Exception(const std::string& msg) : _msg(msg)
    {
    }

    std::string get_msg()
    {
        return _msg;
    }
};

class Tuner {

private:
    struct TunerConfig {
        std::array<size_t, 3> block_size;
        std::array<size_t, 3> grid_size;
    };


    Kernel _kernel;
    bool _tuning = true;
    std::vector<TunerConfig> _configs;
    int _idx = 0;

    int _best_config_idx = 0;
    uint64_t _best_time = UINT64_MAX;
    int _iteration = 0;

public:

    Tuner() {}
    Tuner(Kernel kernel, std::vector<std::array<size_t, 3>> block_sizes, std::vector<std::array<size_t, 3>> grid_sizes);

    bool tuning();

    void run();

    void reset();
};

}

#endif