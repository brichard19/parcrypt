#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include "fmt/format.h"

#include "http_client.h"
#include "config.h"
#include "gpulib.h"

#include "logger.h"
#include "platform.h"
#include "util.h"
#include "work_unit_manager.h"

#include "worker_thread.h"

namespace {

// Global variables
bool _running = true;

std::vector<WorkerThreadState*> _thread_states;
std::mutex _thread_state_mutex;

parcrypt::WorkUnitManager* _manager = nullptr;

Config _config;

std::string to_mkeys(double key_rate)
{
  double mkeys = key_rate / 1e6f;
  std::string rate_str;
  if(mkeys <= 0.1) {
    rate_str = "< 0.1 ";
  } else {
    rate_str = util::format(mkeys, 2);
  }

  return rate_str + " MK/s";
}

struct WorkerSlot {
  int id;
  bool in_use = false;
  ThreadType type;
  GPUConfig gpu;
};

std::vector<WorkerSlot> _worker_slots;


void add_thread(WorkerThreadState* t)
{
  _thread_state_mutex.lock();
  _thread_states.push_back(t);
  _thread_state_mutex.unlock();
}

void remove_thread(WorkerThreadState* t)
{
  _thread_state_mutex.lock();
  // TODO: Not thread-safe
  // Remove thread state
  for(int i = 0; i < _thread_states.size(); i++) {
    if(_thread_states[i] == t) {
      _thread_states.erase(_thread_states.begin() + i);
      break;
    }
  }
  _thread_state_mutex.unlock();
}

} // namespace


bool have_enough_work()
{
  return _manager->total_work_available() >= _worker_slots.size();
}


parcrypt::IWorkItem* get_work()
{
  return _manager->get_work();
}


void return_work(parcrypt::IWorkItem* work_item)
{
  _manager->report_work(work_item);
}


bool get_work_from_server()
{
  // Get work from server
  LOG(LogLevel::Info) << "Retrieving work from sever";

  parcrypt::WorkData work_data;

  HTTPClient client(_config.hostname, _config.port, _config.auth);

  std::vector<std::string> projects = _config.projects;

  // If no projects are listed, take any project
  if(projects.size() == 0) {
    projects.push_back("*");
  }

  // TODO: Determine which project should request work
  for(auto project : projects) {
    ServerResponse response = client.get_work(project, work_data);
    if(response != ServerResponse::Ok) {
      LOG(LogLevel::Error) << to_friendly_string(response);
      continue;
    }

    std::string type = work_data.type;

    auto factory = parcrypt::work_unit_factories();
    if(factory.find(type) == factory.end()) {
      throw std::runtime_error(fmt::format("Invalid work unit type: {0}", type));
    }

    parcrypt::IWorkUnit* work_unit = factory[type]();
    work_unit->create(work_data);

    _manager->add(work_unit);

    return true;
  }

  return false;
}


void worker_thread(WorkerSlot* d, parcrypt::IWorkItem* work_item_ptr)
{
  std::string type = work_item_ptr->parent()->type();

  auto f = parcrypt::worker_factories();

  if(f.find(type) == f.end()) {
    d->in_use = false;
    LOG(LogLevel::Error) << "Unknown work unit type: " << type;
    return;
  }

  WorkerThreadState* state = nullptr;
  
  try {
    state = new WorkerThreadState(d->gpu);
  } catch(gpulib::Exception& ex) {
    LOG(LogLevel::Error) << "GPU error: " << ex.get_msg();
    return;
  }

  state->set_run_flag(&_running);
  state->set_state(WorkerState::Idle);
  state->set_work_item(work_item_ptr);

  add_thread(state);

  try {
    LOG(LogLevel::Info) << d->gpu.device_info.device_name << " starting " << work_item_ptr->parent()->friendly_id();
    f[type](state, work_item_ptr);
  } catch(gpulib::Exception& e) {
    work_item_ptr->set_status(parcrypt::IWorkItem::Status::Aborted);
    LOG(LogLevel::Error) << "GPU error: " << e.get_msg();
  } catch(std::exception& e) {
    work_item_ptr->set_status(parcrypt::IWorkItem::Status::Aborted);
    LOG(LogLevel::Error) << "Error: " << e.what();
  } catch(...) {
    work_item_ptr->set_status(parcrypt::IWorkItem::Status::Aborted);
    LOG(LogLevel::Error) << "Error: Unkown error";
  }

  state->set_work_item(nullptr);
  return_work(work_item_ptr);

  // Remove thread state
  remove_thread(state);

  delete state;
  d->in_use = false;
}

void start_worker_thread(WorkerSlot* d, parcrypt::IWorkItem* work_item)
{
  d->in_use = true;
  std::thread t(worker_thread, d, work_item);
  t.detach();
}

void work_manager_thread_function()
{
  try {
    while(_running) {
      for(int i = 0; i < _worker_slots.size(); i++) {
        if(_worker_slots[i].in_use == false) {
          auto work_item = get_work();

          if(work_item != nullptr) {
            start_worker_thread(&_worker_slots[i], work_item);
          }
        }
      }

      platform::sleep(1.0);
    }
  } catch(...) {
    LOG(LogLevel::Error) << "work manager thread died";
    _running = false;
  }
}

// Retrieves work from the server
void download_thread_function()
{
  try {
    util::Timer retry_timer;

    bool retry = false;
    while(_running) {
      if(!have_enough_work()) {
        if(retry == false || (retry == true && retry_timer.elapsed() >= 30.0)) {

          try {
            if(get_work_from_server()) {
              retry = false;
            } else {
              LOG(LogLevel::Info) << "No work available. Retrying...";
              retry = true;
              retry_timer.start();
            }
          } catch(std::exception& ex) {
            LOG(LogLevel::Info) << "Error connecting to server: " << ex.what();
            LOG(LogLevel::Info) << "Retrying...";
            retry = true;
            retry_timer.start();
          }
        }
      }

      platform::sleep(10.0, &_running);
    }
  } catch(std::exception& e) {
    LOG(LogLevel::Error) << "Download thread died: " << e.what();
    _running = false;
  }

}


// Uploads completed work to the server
void upload_thread_function()
{
  try {
    while(_running) {

      std::vector<parcrypt::CompletedWorkData> completed_work = _manager->get_completed_work();

      for(auto& w : completed_work) {

        // Attempt to upload completed work units
        try {
          LOG(LogLevel::Info) << "Uploading results for work unit " << w.id;
          HTTPClient client(_config.hostname, _config.port, _config.auth);

          client.report_results(w);

          _manager->remove(w.id);
        } catch(std::exception& ex) {
          LOG(LogLevel::Info) << "Error connecting to server: " << ex.what();
        }
      }

      platform::sleep(30.0, &_running);
    }
  } catch(std::exception& e) {
    LOG(LogLevel::Error) << "Upload thread died: " << e.what();
    _running = false;
  }
}



// Displays program status on an interval
// TODO: How should status be displayed? Currently we display the work
// unit name and under it the GPUs that are processing that work
// unit.
void status_thread_function()
{
  class WorkStatusInfo {
  public:
    std::string project_name;
    std::string id;
    std::vector<WorkerThreadState*> states;
  };

  try {
    while(_running) {
      platform::sleep(5.0, &_running);

      int gpus = 0;
      int cpus = 0;

      for(auto& d : _worker_slots) {
        if(d.in_use) {
          if(d.type == ThreadType::GPU) {
            gpus++;
          } else {
            cpus++;
          }
        }
      }

      LOG(LogLevel::Info) << "GPUs: " << gpus << " CPUs: " << cpus;

      // Nothing to do if no threads are running
      if(_thread_states.size() == 0) {
        continue;
      }

      // Build map of project names -> worker threads
      std::map<std::string, WorkStatusInfo> work_status;

      // TODO: Find better way to do this
      _thread_state_mutex.lock();
      for(auto& state : _thread_states) {
        if(state->work_item() == nullptr) {
          continue;
        }
        std::string name = state->work_item()->parent()->name();
        std::string id = state->work_item()->parent()->friendly_id();
        std::string key = name + "-" + id;
        if(work_status.find(key) != work_status.end()) {
          work_status[key].states.push_back(state);
        } else {
          WorkStatusInfo info;
          info.project_name = name;
          info.id = id;
          info.states.push_back(state);
          work_status.insert(std::pair<std::string, WorkStatusInfo>(key, info));
        }
      }
      _thread_state_mutex.unlock();

      if(work_status.size() > 0) {
        std::stringstream log_string;
        for(auto& status : work_status) {
          log_string << "Project: " << status.second.project_name << " Work unit: " << status.second.id << std::endl;
          for(auto& thread_state : status.second.states) {
            std::string gpu_name = util::pad_string(thread_state->gpu_config.device_info.device_name, 24);
            log_string << "  " << gpu_name << " ";

            switch(thread_state->state()) {
            case WorkerState::Running:

              log_string << to_mkeys(thread_state->rate());
              break;
            case WorkerState::Idle:
              log_string << "Idle         ";
              break;
            case WorkerState::Initializing:
              log_string << "Initializing ";
              break;
            }

            // End of line
            log_string << std::endl;
          }
        }
        LOG(LogLevel::Info) << log_string.str();
      }
    }
  } catch(std::exception& ex) {
    LOG(LogLevel::Error) << "Status thread died: " << ex.what();
    _running = false;
  } catch(...) {
    LOG(LogLevel::Error) << "Status thread died";
    _running = false;
  }
}


// Initialize the GPU threads
void init(std::vector<GPUConfig>& selected_devices)
{
  int i = 0;
  for(auto& gpu_config : selected_devices) {
    WorkerSlot d;
    d.id = i;
    d.in_use = false;
    d.type = ThreadType::GPU;
    d.gpu = gpu_config;

    _worker_slots.push_back(d);
    i++;
  }
}


// Run the GPU threads and the status thread. This function blocks
// until all threads have completed.
void run()
{
  // Start download thread
  std::thread download_thread(download_thread_function);

  // Start upload thread
  std::thread upload_thread(upload_thread_function);

  // Start work manager thread
  std::thread work_manager_thread(work_manager_thread_function);

  // Start status thread
  std::thread status_thread(status_thread_function);

  // Wait for worker threads to finish
  // TODO: Find a better way to do this
  while(_thread_states.size() > 0) {
    platform::sleep(1.0);
  }

  // Wait for work manager thread to finish
  if(work_manager_thread.joinable()) {
    work_manager_thread.join();
  }

  // Wait for download thread to finish
  if(download_thread.joinable()) {
    download_thread.join();
  }

  // Wait for upload thread to finish
  if(upload_thread.joinable()) {
    upload_thread.join();
  }

  // Wait for status thread to finish
  if(status_thread.joinable()) {
    status_thread.join();
  }
}


// Handles the CTRL-c interrupt
void console_handler(int signal)
{
  (void)signal;

  _running = false;

  LOG(LogLevel::Info) << "Received termination signal. Exiting..." << std::endl;
}


int main(int argc, char** argv)
{
  std::string config_file = "config.json";

  // Currently the only argument accepted is --config
  if(argc > 1) {
    if(argc == 3 && strcmp(argv[1], "--config") == 0) {
      config_file = argv[2];
    } else {
      std::cout << "Usage:" << std::endl;
      std::cout << "--config FILE" << std::endl;
      std::cout << "    Read configuration from FILE" << std::endl;
      return 1;
    }
  }

  std::vector<gpulib::DeviceInfo> selected_devices;

  try {
    _config = load_config(config_file);
  } catch(std::exception& e) {
    std::cout << "Configuration error: " << e.what() << std::endl;
    return 1;
  }

  try {
    _manager = new parcrypt::WorkUnitManager(_config.data_dir);
  } catch(std::exception& ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }

  std::vector<gpulib::DeviceInfo> available_gpus = gpulib::get_devices();

  if(available_gpus.size() == 0) {
    std::cout << "Error: No GPUs available" << std::endl;
    return 1;
  }

  std::cout << "Found the following GPUs: " << std::endl;
  for(auto& gpu : available_gpus) {
    std::cout << gpu.device_name << std::endl;
  }

  for(auto& gpu_config : _config.gpu_devices) {
    if(gpu_config.device_id > available_gpus.size()) {
      std::cout << "Invalid device id " << std::to_string(gpu_config.device_id) << std::endl;
      return 1;
    }
    gpu_config.device_info = available_gpus[gpu_config.device_id];
  }

  init(_config.gpu_devices);

  // Set handler to handle CTRL-C signal
  platform::set_console_handler(console_handler);

  run();

  delete _manager;

  return 0;
}
