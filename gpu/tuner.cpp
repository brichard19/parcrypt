#include "gpulib_interface.h"

#include <chrono>

gpulib::Tuner::Tuner(Kernel kernel,
  std::vector<std::array<size_t, 3>> local_sizes,
  std::vector<std::array<size_t, 3>> grid_sizes)
  : _kernel(kernel)
{
  for(auto& grid_size : grid_sizes) {
    for(auto& local_size : local_sizes) {
      TunerConfig config;
      config.grid_size = grid_size;
      config.block_size = local_size;
      _configs.push_back(config);
    }
  }
}

bool gpulib::Tuner::tuning()
{
  return _tuning;
}

void gpulib::Tuner::run()
{
  if(!_tuning) {
    return;
  }

  _kernel->set_block_size(_configs[_idx].block_size);
  _kernel->set_grid_size(_configs[_idx].grid_size);

  uint64_t t0 = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  _kernel->call(true);
  uint64_t t = std::chrono::high_resolution_clock::now().time_since_epoch().count() - t0;

  if(t < _best_time) {
    _best_time = t;
    _best_config_idx = _idx;
  }

  if(++_iteration == 10) {
    _iteration = 0;
    if(++_idx == _configs.size()) {
      _tuning = false;
      _kernel->set_block_size(_configs[_best_config_idx].block_size);
      _kernel->set_grid_size(_configs[_best_config_idx].grid_size);
    }
  }
}