#ifndef _WORKER_THREAD_H
#define _WORKER_THREAD_H

#include <algorithm>

#include "gpu_config.h"
#include "worker_device.h"
#include "work_unit.h"

enum class WorkerState {
  Idle,
  Running,
  Initializing
};

enum class ThreadType {
  CPU,
  GPU
};

class WorkerThreadState {

private:
  ThreadType _type;
  volatile double _key_rate = 0.0;
  volatile float _progress = 0.0f;

  volatile bool* _run_flag = nullptr;
  WorkerState _worker_state = WorkerState::Idle;

  parcrypt::IWorkItem* _work_item = nullptr;

public:

  GPUConfig gpu_config;
  gpulib::Device gpu;




  WorkerThreadState(ThreadType type) :  _type(type)
  {
  }

  WorkerThreadState(GPUConfig gpu) : _type(ThreadType::GPU), gpu_config(gpu)
  {
    this->gpu = gpulib::get_device(gpu_config.device_info);
  }

  void set_run_flag(bool* flag_ptr)
  {
    _run_flag = flag_ptr;
  }

  bool running()
  {
    return *_run_flag;
  }

  float progress()
  {
    return _progress;
  }

  double rate()
  {
    return _key_rate;
  }

  void set_progress(float progress)
  {
    _progress = std::clamp(progress, 0.0f, 1.0f);
  }

  void set_rate(double rate)
  {
    _key_rate = std::max(0.0, rate);
  }

  void set_state(WorkerState state)
  {
    _worker_state = state;
  }

  WorkerState state()
  {
    return _worker_state;
  }

  parcrypt::IWorkItem* work_item()
  {
    return _work_item;
  }

  void set_work_item(parcrypt::IWorkItem* work_item)
  {
    _work_item = work_item;
  }
};

#endif
