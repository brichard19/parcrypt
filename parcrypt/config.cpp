#include <exception>
#include "config.h"
#include "json11.hpp"
#include "logger.h"
#include "platform.h"
#include "util.h"

namespace {

std::string sanitize_json(const std::string& json)
{
  // Replace single backslash with double backslash
  size_t idx = 0;
  std::string s = json;
  while((idx = s.find("\\", idx)) != std::string::npos) {
    s.insert(idx, "\\");
    idx += 2;
  }

  return s;
}

}

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
  std::string str = sanitize_json(buffer.str());
  std::string err;
  const auto json = json11::Json::parse(str, err);

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

  if(json.object_items().count("data_dir")) {
    config.data_dir = json["data_dir"].string_value();
  }

  for(auto i : json["gpu_devices"].array_items()) {
    GPUConfig gpu_config;
    auto items = i.object_items();
    if(items.find("opencl") != items.end()) {
      gpu_config.device_id = items["opencl"].int_value();
      gpu_config.api = (int)gpulib::DeviceType::OpenCL;
    } else if(items.find("cuda") != items.end()) {
      gpu_config.device_id = items["cuda"].int_value();
      gpu_config.api = (int)gpulib::DeviceType::CUDA;
    } else {
      throw std::runtime_error("expected 'opencl' or 'cuda'");
    }

    // Ensure no duplicates
    bool skip = false;
    for(auto& d : config.gpu_devices) {
      if(d.api == gpu_config.api && d.device_id == gpu_config.device_id) {
        LOG(LogLevel::Warning) << "Duplicate device in config. Skipping";
        skip = true;
        break;
      }
    }

    if(skip) {
      continue;
    }

    // Parse memory usage as a percent or number of bytes
    if(items.find("mem_usage") != items.end()) {
      double value = 0.0f;
      std::string s = items["mem_usage"].string_value();

      if(util::parse_bytes(s, &value)) {
        gpu_config.mem_usage = MemUsage(MemUsage::MemUsageType::Bytes, value);
      } else if(util::parse_percent(s, &value)) {
        gpu_config.mem_usage = MemUsage(MemUsage::MemUsageType::Percent, value / 100.0);
      } else {
        LOG(LogLevel::Error) << "mem_usage must be a percentage or size e.g. \"85%\" or \"4GB\"";
        throw std::runtime_error("mem_usage");
      }
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