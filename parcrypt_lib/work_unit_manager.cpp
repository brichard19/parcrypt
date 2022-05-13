#include "work_unit_manager.h"

#include <exception>
#include "logger.h"
#include "platform.h"

namespace parcrypt {


WorkUnitManager::WorkUnitManager(const std::string& data_dir)
: _data_dir(data_dir)
{
  _work_dir = platform::make_path(_data_dir, WORK_DIR_NAME);
  _completed_dir = platform::make_path(_data_dir, COMPLETED_DIR_NAME);

  if(!platform::exists(data_dir)) {
    platform::create_directories(data_dir);
  }

  // Create directories
  platform::create_directories(_work_dir);
  platform::create_directories(_completed_dir);

  load_work();
}

WorkUnitManager::~WorkUnitManager()
{
  for(auto& wu : _work_units) {
    delete wu.second;
  }
}

void WorkUnitManager::load_work()
{
  auto directories = platform::get_directories(_work_dir);

  auto factories = work_unit_factories();

  for(auto d : directories) {
    std::string type = platform::file_name(d);

    if(factories.find(type) == factories.end()) {
      throw std::runtime_error("Unknown work unit type");
    }

    auto files = platform::get_files(d, ".wrk");

    for(auto& f : files) {

      IWorkUnit* wu = factories[type]();
      wu->load(f);
      _work_units.insert(std::pair<std::string, IWorkUnit*>(wu->id(), wu));
    }
  }
}

IWorkItem* WorkUnitManager::get_work()
{
  IWorkItem* item = nullptr;

  _mutex.lock();

  for(auto& wu : _work_units) {
    if(!wu.second->done()) {
      item = wu.second->get_work();
      if(item) {
        break;
      }
    }
  }

  _mutex.unlock();
  return item;
}

void WorkUnitManager::report_work(IWorkItem* work_item)
{
  _mutex.lock();

  std::string id = work_item->parent()->id();

  if(_work_units.find(id) != _work_units.end()) {
    _work_units[id]->report_work(work_item);

    std::string dir = platform::make_path(_work_dir, _work_units[id]->type());
    platform::create_directories(dir);
    _work_units[id]->save(dir + "/" + id + ".wrk");
  }

  _mutex.unlock();
}

void WorkUnitManager::add(IWorkUnit* wu)
{
  _mutex.lock();

  std::string id = wu->id();

  // TODO: What if the work unit already exists?
  if(_work_units.find(id) == _work_units.end()) {
    _work_units.insert(std::pair<std::string, IWorkUnit*>(id, wu));

    std::string dir = platform::make_path(_work_dir, _work_units[id]->type());
    platform::create_directories(dir);
    _work_units[id]->save(platform::make_path(dir, id + ".wrk"));
  } else {
    LOG(LogLevel::Warning) << "Work unit " << id << " already exists";
  }

  _mutex.unlock();
}

void WorkUnitManager::remove(const std::string& id)
{
  _mutex.lock();

  if(_work_units.find(id) != _work_units.end()) {

    std::string type = _work_units[id]->type();
    platform::create_directories(_completed_dir + "/" + type);

    // Move to completed folder
    platform::rename(platform::make_path(_work_dir, type, id + ".wrk"), platform::make_path( _completed_dir, type, id + ".wrk" ));

    IWorkUnit* ptr = _work_units[id];
    delete ptr;
    _work_units.erase(id);
  }
  _mutex.unlock();
}

std::vector<CompletedWorkData> WorkUnitManager::get_completed_work()
{
  std::vector<CompletedWorkData> completed_work;

  _mutex.lock();
  for(auto& wu : _work_units) {
    if(wu.second->done()) {
      CompletedWorkData cw;
      cw.project = wu.second->name();
      cw.type = wu.second->type();
      cw.id = wu.second->id();
      cw.payload = wu.second->get_results();
      completed_work.push_back(cw);
    }
  }

  _mutex.unlock();

  return completed_work;
}

int WorkUnitManager::total_work_available()
{
  int total = 0;

  _mutex.lock();
  for(auto& wu : _work_units) {
    total += wu.second->available_work();
  }
  _mutex.unlock();

  return total;
}

}