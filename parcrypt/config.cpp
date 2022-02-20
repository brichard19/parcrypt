#include <exception>

#include "config.h"

#include "json11.hpp"

#include "platform.h"

Config load_config(const std::string& path)
{
  Config config;

  config.projects.clear();

  if(!platform::file_exists(path)) {
    throw std::runtime_error(path + " not found");
  }

  std::ifstream f(path.c_str());

  if(f.fail()) {
    throw std::runtime_error("Error opening" + path);
  }

  std::stringstream buffer;
  buffer << f.rdbuf();
  std::string err;
  const auto json = json11::Json::parse(buffer.str(), err);

  config.hostname = json["hostname"].string_value();
  config.port = json["port"].int_value();

  // TODO: Add support for user authentication on server
  //if(json["auth_token"].is_null()) {
  //  config.username = json["user"].string_value();
  //  config.password = json["password"].string_value();
  //} else {
  //  config.auth_token = json["auth_token"].string_value();
  //}


  // TODO: Allow the config file to specify which projects to work on
  //for(auto i : json["projects"].array_items()) {
  //  config.projects.push_back(i.string_value());
  //}

  for(auto i : json["gpu_devices"].array_items()) {
    GPUConfig gpu_config;
    auto items = i.object_items();
    if(items.find("opencl") != items.end()) {
      gpu_config.device_id = items["opencl"].int_value();
    } else if(items.find("cuda") != items.end()) {
      gpu_config.device_id = items["cuda"].int_value();
    } else {
      throw std::runtime_error("expected 'opencl' or 'cuda'");
    }

    if(items.find("mem_usage") != items.end()) {
      gpu_config.mem_usage = (float)items["mem_usage"].number_value();
    }

    config.gpu_devices.push_back(gpu_config);
  }

  if(!config.auth_token.empty()) {
    config.auth = ClientAuth(config.auth_token);
  } else {
    config.auth = ClientAuth(config.username, config.password);
  }

  return config;
}