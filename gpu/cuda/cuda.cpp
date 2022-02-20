#include "gpulib_cuda.h"


static bool _cuda_initialized = false;


void cu_call(CUresult err)
{
    if(err != CUDA_SUCCESS) {
        throw gpulib::Exception("CUDA error: " + std::to_string(err));
    }
}

static void initialize_cuda()
{
    if(!_cuda_initialized) {
        cu_call(cuInit(0));
        _cuda_initialized = true;
    }
}


gpulib::cuda::CUDADevice::CUDADevice(int device_id) : _device_id(device_id)
{
    cu_call(cuDeviceGet(&_device, device_id));
    cu_call(cuCtxCreate(&_ctx, 0, _device));
}

gpulib::cuda::CUDADevice::~CUDADevice()
{
    cuCtxDestroy(_ctx);
}

std::shared_ptr<gpulib::IDeviceMemory> gpulib::cuda::CUDADevice::malloc(size_t size)
{
    std::shared_ptr<IDeviceMemory> ptr;

    CUDADeviceMemory* mem = new CUDADeviceMemory(_device, size);

    ptr.reset(mem);

    return ptr;
}

std::shared_ptr<gpulib::IKernel> gpulib::cuda::CUDADevice::load_kernel_from_source(const std::string& src, const std::string& entry)
{
    std::shared_ptr<IKernel> ptr;

    return ptr;
}

std::shared_ptr<gpulib::IKernel> gpulib::cuda::CUDADevice::load_kernel_from_file(const std::string& file_path, const std::string& entry)
{
    CUmodule module;

    if(_modules.find(file_path) == _modules.end()) {
        cu_call(cuModuleLoad(&module, file_path.c_str()));

        _modules.insert(std::pair<std::string, CUmodule>(file_path, module));
    } else {
        module = _modules[file_path];
    }

    CUfunction function;
    cu_call(cuModuleGetFunction(&function, module, entry.c_str()));

    std::shared_ptr<IKernel> ptr;
    ptr.reset(new CUDAKernel(function));

    return ptr;
}

std::shared_ptr<gpulib::IKernel> gpulib::cuda::CUDADevice::load_kernel_from_source(const std::string& entry, const std::string& src, const std::string& module_name)
{
    std::shared_ptr<IKernel> ptr;

    return ptr;
}

void gpulib::cuda::CUDAKernel::set_block_size(std::array<size_t, 3> block_size)
{
    _block_size = block_size;
}

void gpulib::cuda::CUDAKernel::set_grid_size(std::array<size_t, 3> grid_size)
{
    _grid_size = grid_size;
}

size_t gpulib::cuda::CUDADevice::get_global_mem_size()
{
    return 0;
}

std::array<size_t, 3> gpulib::cuda::CUDADevice::get_max_local_dim()
{
    return { {1,1,1} };
}

size_t gpulib::cuda::CUDADevice::get_max_local_size()
{
    return 0;
}

size_t gpulib::cuda::CUDADevice::get_mp_count()
{
    return _mp_count;
}

gpulib::cuda::CUDADeviceMemory::CUDADeviceMemory(CUdevice device, size_t size) : _device(device), _size(size)
{
    cu_call(cuMemAlloc(&_mem, size));
}

gpulib::cuda::CUDADeviceMemory::~CUDADeviceMemory()
{
    if(_mem) {
        cuMemFree(_mem);
    }
}

void gpulib::cuda::CUDADeviceMemory::read(void* dst, size_t count)
{
    cu_call(cuMemcpyDtoH(dst, _mem, count));
}

void gpulib::cuda::CUDADeviceMemory::read(void* dst, size_t offset, size_t count)
{
    cu_call(cuMemcpyDtoH(dst, _mem + offset, count));
}


void gpulib::cuda::CUDADeviceMemory::write(const void* src, size_t count)
{
    cu_call(cuMemcpyHtoD(_mem, src, count));
}

void gpulib::cuda::CUDADeviceMemory::write(const void* src, size_t offset, size_t count)
{
    cu_call(cuMemcpyHtoD(_mem + offset, src, count));
}

void gpulib::cuda::CUDADeviceMemory::memset(uint32_t value)
{
    cu_call(cuMemsetD32(_mem, value, _size / sizeof(value)));
}

size_t gpulib::cuda::CUDADeviceMemory::size()
{
    return _size;
}

gpulib::cuda::CUDAKernel::CUDAKernel(CUfunction function) : _function(function)
{
}

gpulib::cuda::CUDAKernel::~CUDAKernel()
{
}

void gpulib::cuda::CUDAKernel::call()
{
    std::vector<void*> arg_ptrs;

    size_t num_args = _args.size();
    for(int i = 0; i < num_args; i++) {
        arg_ptrs.push_back(_args[i].data());
    }

    cu_call(cuLaunchKernel(_function,
        static_cast<unsigned int>(_grid_size[0]),
        static_cast<unsigned int>(_grid_size[1]),
        static_cast<unsigned int>(_grid_size[2]),
        static_cast<unsigned int>(_block_size[0]),
        static_cast<unsigned int>(_block_size[1]),
        static_cast<unsigned int>(_block_size[2]),
        0,
        0,
        arg_ptrs.data(),
        nullptr));
    cu_call(cuCtxSynchronize());
}

void gpulib::cuda::CUDAKernel::set_arg(int arg_idx, const void* arg, size_t size)
{
    std::vector<uint8_t> value(size);
    std::memcpy(value.data(), arg, size);

    _args[arg_idx] = value;
}

void gpulib::cuda::CUDAKernel::set_arg(int arg_idx, std::shared_ptr<IDeviceMemory> buffer)
{
    CUDADeviceMemory* mem_obj = static_cast<CUDADeviceMemory*>(buffer.get());

    CUdeviceptr dev_ptr = mem_obj->get_mem();

    std::vector<uint8_t> value(sizeof(dev_ptr));
    std::memcpy(value.data(), &dev_ptr, sizeof(dev_ptr));

    _args[arg_idx] = value;
}

std::vector<gpulib::DeviceInfo> gpulib::cuda::get_cuda_devices()
{
    initialize_cuda();

    std::vector<gpulib::DeviceInfo> device_list;
    int device_count = 0;

    cu_call(cuDeviceGetCount(&device_count));

    if(device_count == 0) {
        return device_list;
    }

    for(int device_id = 0; device_id < device_count; device_id++) {
        CUdevice d;
        gpulib::DeviceInfo info;
        info.type = gpulib::CUDA;
        info.vendor_name = "NVIDIA Corporation";
        cu_call(cuDeviceGet(&d, device_id));

        info.cuda_device_id = device_id;
        
        // Get device name
        int name_len = 0;
        char name[128] = { 0 };
        cu_call(cuDeviceGetName(name, sizeof(name), d));
        info.device_name = std::string(name);

        size_t mem_size = 0;
        cu_call(cuDeviceTotalMem(&mem_size, d));
        info.mem_size = (uint64_t)mem_size;

        device_list.push_back(info);
    }

    return device_list;
}