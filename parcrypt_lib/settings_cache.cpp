#include <fstream>
#include <sstream>

#include "settings_cache.h"
#include "json11.hpp"
#include "fmt/format.h"

SettingsCache::SettingsCache()
{
  _mutex = new std::mutex();
}

SettingsCache::~SettingsCache()
{
  delete _mutex;
}

SettingsCache& SettingsCache::instance()
{
  static SettingsCache static_instance;

  return static_instance;
}


bool SettingsCache::exists(const std::string& key)
{
  _mutex->lock();
  bool exists = _map.find(key) != _map.end();
  _mutex->unlock();

  return exists;
}

std::string SettingsCache::get(const std::string& key)
{
  if(!exists(key)) {
    return "";
  }

  return _map[key];
}

void SettingsCache::set(const std::string& key, const std::string& value)
{
  if(exists(key)) {
    _map[key] = value;
  } else {
    _map.insert(std::pair(key, value));
  }
}