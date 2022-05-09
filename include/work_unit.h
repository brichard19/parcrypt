#ifndef _WORK_UNIT_H
#define _WORK_UNIT_H

#include <string>
#include <vector>
#include <map>
#include <set>

#include "json11.hpp"
#include "secp256k1.h"

namespace parcrypt {

class IWorkUnit;

class IWorkItem {


public:
  enum Status {
    New = 1,
    Completed = 2,
    Aborted = 3
  };

  int _status = New;

  IWorkUnit* _parent = nullptr;

  virtual ~IWorkItem() {}

  virtual int id() = 0;

  IWorkUnit* parent()
  {
    return _parent;
  }

  void set_status(int status)
  {
    _status = status;
  }

  int status()
  {
    return _status;
  }
};

struct WorkData {
  std::string type;
  std::string name;
  std::string work_id;
  json11::Json payload;
};

struct CompletedWorkData {
  std::string project;
  std::string type;
  std::string id;
  json11::Json payload;
};

class IWorkUnit {

protected:

  std::string _name;

  std::string _id;

public:

  IWorkUnit() = default;

  virtual ~IWorkUnit() {}

  virtual std::string type() = 0;

  virtual std::string name() = 0;

  virtual std::string id() = 0;

  virtual std::string friendly_id() = 0;

  virtual void create(const WorkData& work_data) = 0;

  virtual void save(const std::string& path) = 0;

  virtual void load(const std::string& path) = 0;

  virtual bool done() = 0;

  virtual IWorkItem* get_work() = 0;

  virtual void report_work(IWorkItem* work_item) = 0;

  virtual int available_work() = 0;

  virtual json11::Json get_results() = 0;
};

IWorkUnit* work_unit_factory(WorkData& work_data);

}
#endif
