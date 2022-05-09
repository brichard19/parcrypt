#ifndef _BRUTE_FORCE_WORK_UNIT_H
#define _BRUTE_FORCE_WORK_UNIT_H

#include "work_unit.h"
#include "brute_force_work_item.h"

namespace parcrypt {

class BruteForceWorkUnit : public IWorkUnit {

private:

  uint64_t _share_size = (uint64_t)1 << 36;

  std::string _name;

  std::string _id;

  std::string _target;

  secp256k1::uint256 _start;

  secp256k1::uint256 _end;

  std::string _private_key = "";

  std::map<int, BruteForceWorkItem*> _available_work;

  std::set<int> _completed_work;

  std::set<int> _outstanding_work;

  void init();

protected:

  void load(const json11::Json& data);

  void create(const json11::Json& data);

  json11::Json save();

public:

  BruteForceWorkUnit() = default;

  virtual ~BruteForceWorkUnit();

  void create(const WorkData & work_data);

  void save(const std::string & path);

  void load(const std::string & path);

  bool done();

  std::string name();

  std::string id();

  std::string friendly_id();

  IWorkItem* get_work();

  int available_work();

  void report_work(IWorkItem * item);

  json11::Json get_results();
};

}

#endif