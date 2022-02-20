#ifndef _SETTINGS_CACHE_H
#define _SETTINGS_CACHE_H

#include <map>
#include <mutex>
#include <string>

class SettingsCache {

private:
  std::mutex* _mutex = nullptr;
  std::map<std::string, std::string> _map;

public:
  SettingsCache();
  SettingsCache(SettingsCache&) = delete;
  void operator=(SettingsCache&) = delete;

  ~SettingsCache();

  bool exists(const std::string& key);

  std::string get(const std::string& key);

  void set(const std::string& key, const std::string& value);

  static SettingsCache& instance();
};

#endif
