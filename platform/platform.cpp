
#include <cstdio>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <thread>

#include "platform.h"


namespace {

bool ends_with(const std::string& str, const std::string& suffix)
{
  if(suffix.length() > str.length()) {
    return false;
  }

  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

} // namespace

namespace platform {

std::string make_path_internal(const std::vector<std::string>& elements)
{
#ifdef _WIN32
  const char separator = '\\';
#else
  const char separator = '/';
#endif
  std::string path = "";

  for(int i = 0; i < elements.size() - 1; i++) {
    std::string e = elements[i];

    if(e.empty()) {
      continue;
    }

    if(e.at(e.length() - 1) != separator) {
      e += separator;
    }

    path += e;
  }
  
  path += elements[elements.size() - 1];

  return path;
}


bool file_exists(const std::string& file_path)
{
  return std::filesystem::exists(file_path);

}

bool exists(const std::string& path)
{
  return std::filesystem::exists(path);
}

void rename(const std::string& old_path, const std::string& new_path)
{
  std::filesystem::rename(old_path, new_path);
}

bool create_directories(const std::string& path)
{
  return std::filesystem::create_directories(path);
}

void append_to_file(const std::string& file_path, const void* data, size_t len)
{
  std::ofstream f(file_path, std::ios::binary | std::ios::app);
  f.write(static_cast<const char*>(data), len);
}

void read_file_into_memory(const std::string& file_name, char** ptr, size_t* size_ptr)
{
  std::ifstream f(file_name, std::ios::binary | std::ios::ate);
  std::streamsize size = f.tellg();
  f.seekg(0, std::ios::beg);

  char* buffer = new char[size];

  f.read((char*)buffer, size);

  *ptr = buffer;
  *size_ptr = size;
}

void delete_file(const std::string& file_name)
{
  std::filesystem::remove(file_name);
}

void sleep(double seconds)
{
  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(seconds * 1000.0));
}

// Interruptable sleep
void sleep(double seconds, volatile bool* condition)
{
  int count = static_cast<int>(seconds);
  double remaining = seconds - floor(seconds);

  for(int i = 0; i < count; i++) {
    if(*condition == false) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000.0));
  }

  if(remaining > 0.0) {
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(remaining * 1000.0));
  }
}

std::vector<std::string> get_files(const std::string& dir, const std::string& ext)
{
  std::vector<std::string> file_names;

  for(const auto& entry : std::filesystem::directory_iterator(dir)) {
    if(!ext.empty()) {
      if(ends_with(entry.path().string(), ext)) {
        file_names.push_back(entry.path().string());
      }
    } else {
      file_names.push_back(entry.path().string());
    }
  }

  return file_names;
}

std::vector<std::string> get_directories(const std::string& dir)
{
  std::vector<std::string> dir_names;

  for(const auto& entry : std::filesystem::directory_iterator(dir)) {
    if(entry.is_directory()) {
      dir_names.push_back(entry.path().string());
    }
  }

  return dir_names;
}

std::string file_name(const std::string& path)
{
  size_t idx = path.find_last_of('/');

  if(idx == std::string::npos) {
    idx = path.find_last_of('\\');
    if(idx == std::string::npos) {
      return path;
    }
  }

  return path.substr(idx + 1);
}

}