#ifndef _WORK_UNIT_MANAGER_H
#define _WORK_UNIT_MANAGER_H

#include <map>
#include <mutex>
#include <vector>

#include "work_unit.h"

#include "worker_thread.h"

namespace parcrypt {

class WorkUnitManager {

private:
  std::mutex _mutex;

  const std::string WORK_DIR_NAME = "work_units";
  const std::string COMPLETED_DIR_NAME = "completed_work";

  std::string _work_dir;
  std::string _completed_dir;
  std::string _data_dir;

  std::map<std::string, IWorkUnit*> _work_units;

  void load_work();

public:

  WorkUnitManager(const std::string& data_dir = "");
  
  ~WorkUnitManager();

  // Returns a work item
  IWorkItem* get_work();

  // Report a completed work item
  void report_work(IWorkItem* work_item);

  // Add a new work unit
  void add(IWorkUnit* wu);

  // Remove a work unit
  void remove(const std::string& id);

  // Returns a vector containing the result of the work unit
  std::vector<CompletedWorkData> get_completed_work();

  // Total number of work items available
  int total_work_available();
};

typedef IWorkUnit* (*WorkUnitFactory)();

const std::map<std::string, WorkUnitFactory>& work_unit_factories();


typedef void (*WorkerFactory)(WorkerThreadState*, parcrypt::IWorkItem*);

const std::map<std::string, WorkerFactory>& worker_factories();

}
#endif
