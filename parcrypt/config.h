#ifndef _CONFIG_H
#define _CONFIG_H

#include "gpu_config.h"
#include "gpulib.h"
#include "http_client.h"

class Config {

public:
  std::vector<std::string> projects;
  std::vector<GPUConfig> gpu_devices;
  int cpu_threads = 0;
  std::string hostname;
  int port = 80;
  std::string username;
  std::string password;
  std::string auth_token;
  std::string data_dir;

  // Authentication object
  ClientAuth auth;
};

Config load_config(const std::string& path);

#endif