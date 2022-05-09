#include "brute_force_work_unit.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "json11.hpp"
#include "util.h"


namespace parcrypt {

BruteForceWorkUnit::~BruteForceWorkUnit()
{
  for(auto w : _available_work) {
    delete w.second;
  }
}

std::string BruteForceWorkUnit::name()
{
  return _name;
}

std::string BruteForceWorkUnit::id()
{
  return _id;
}

std::string BruteForceWorkUnit::friendly_id()
{
  return _id.substr(0, 8);
}

void BruteForceWorkUnit::init()
{
  secp256k1::uint256 key = _start;

  int item_id = 0;

  // Divide the keyspace up into shares that are at most
  // _share_size keys. Each share is numbered from 0 to
  // n - 1 (where n is the total number of shares).
  uint64_t total_keys = (_end - _start).to_uint64();

  uint64_t inc = std::min(_share_size, total_keys);

  while(key < _end) {
    secp256k1::uint256 next_key = key + secp256k1::uint256(inc);

    // The last share might be smaller than the others.
    if(next_key > _end) {
      next_key = _end;
    }

    // Add the item to the list of available work if they are not
    // already in the completed work item list.
    if(_completed_work.find(item_id) == _completed_work.end()) {
      BruteForceWorkItem* item = new BruteForceWorkItem(this, item_id, _target, key, next_key);
      _available_work.insert(std::pair<int, BruteForceWorkItem*>(item_id, item));
    }
    key = next_key;
    item_id++;
  }
}

void BruteForceWorkUnit::load(const json11::Json& json)
{
  _name = json["name"].string_value();
  _id = json["id"].string_value();
  _start = secp256k1::uint256(json["start"].string_value());
  _end = secp256k1::uint256(json["end"].string_value());
  _target = json["target"].string_value();
  _share_size = util::parse_uint64(json["share_size"].string_value());
  std::string pk = json["private_key"].string_value();

  if(pk.empty() || pk == "private_key_unknown") {
    _private_key = "";
  } else {
    _private_key = pk;
  }

  // Read the list of completed work items
  auto items = json["completed_work"].array_items();

  for(auto i : items) {
    int value = i.int_value();
    _completed_work.insert(value);
  }

  init();
}

void BruteForceWorkUnit::load(const std::string& path)
{
  std::ifstream f(path);
  std::stringstream buffer;
  buffer << f.rdbuf();
  std::string err;
  const auto json = json11::Json::parse(buffer.str(), err);

  load(json);
}

void BruteForceWorkUnit::create(const json11::Json& payload)
{
  _target = payload["target"].string_value();
  _start = secp256k1::uint256(payload["start"].string_value());
  _end = secp256k1::uint256(payload["end"].string_value());
}

void BruteForceWorkUnit::create(const WorkData& work_data)
{
  _name = work_data.name;
  _id = work_data.work_id;

  create(work_data.payload);

  init();
}

json11::Json BruteForceWorkUnit::save()
{
  json11::Json json = json11::Json::object{
  {"name", _name},
  {"id", _id},
  {"target", _target},
  {"start", _start.to_string()},
  {"end", _end.to_string()},
  {"private_key", _private_key.empty() ? "private_key_unknown" : _private_key},
  {"completed_work", _completed_work},
  {"share_size", std::to_string(_share_size)}
  };

  return json;
}

void BruteForceWorkUnit::save(const std::string& path)
{
  json11::Json json = save();

  std::ofstream f(path, std::ios::out);
  f << json.dump();
}


bool BruteForceWorkUnit::done()
{
  // Done if private key was found
  // TODO: Even if the private key was found, we want to wait for all
  // work items to finish otherwise this work unit might be removed
  // while work items are still running. This will cause a crash because
  // the work items maintain a pointer to the work unit.
  /*
  if(!_private_key.empty()) {
    return true;
  }
  */

  return _available_work.size() == 0;
}


IWorkItem* BruteForceWorkUnit::get_work()
{
  // Done. Don't need to do any more work.
  if(done()) {
    return nullptr;
  }

  // No available work
  if(_available_work.size() == 0) {
    return nullptr;
  }

  // Look for available work that is not currently being worked on.
  for(auto& i : _available_work) {
    if(_outstanding_work.find(i.first) == _outstanding_work.end()) {
      _outstanding_work.insert(i.first);
      return i.second;
    }
  }

  // No work currently available.
  return nullptr;
}


int BruteForceWorkUnit::available_work()
{
  if(done()) {
    return 0;
  }

  return static_cast<int>(_available_work.size() - _outstanding_work.size());
}


void BruteForceWorkUnit::report_work(IWorkItem* ptr)
{
  BruteForceWorkItem* work_item = reinterpret_cast<BruteForceWorkItem*>(ptr);

  int id = work_item->id();

  std::string private_key = work_item->get_private_key();

  if(_outstanding_work.find(id) == _outstanding_work.end()) {
    return;
  }

  if(work_item->status() == IWorkItem::Completed) {
    if(_available_work.find(id) != _available_work.end()) {
      _available_work.erase(id);
    }

    _completed_work.insert(id);

    if(!private_key.empty()) {
      _private_key = private_key;
    }
  } else if(work_item->status() == IWorkItem::Aborted || work_item->status() == IWorkItem::New) {
    // Reset the work status and put it back into the pool of available
    // work.
    work_item->set_status(IWorkItem::New);
    _outstanding_work.erase(id);
  } else {
    throw std::runtime_error("Work item in unknown state");
  }
}

json11::Json BruteForceWorkUnit::get_results()
{
  if(!done()) {
    throw std::runtime_error("get_results() called when work unit is not done");
  }

  auto json = json11::Json::object{
    { "private_key", _private_key },
  };

  return json;
}

}