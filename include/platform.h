#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <functional>
#include <string>
#include <vector>

#include "arg_parse.h"

namespace platform {
std::string make_path_internal(const std::vector<std::string>& elements);
bool file_exists(const std::string& file_path);
void append_to_file(const std::string& file_path, const void* data, size_t len);
bool set_console_handler(std::function<void(int)> handler);
void read_file_into_memory(const std::string& file_name, char** ptr, size_t* size_ptr);
void delete_file(const std::string& file_name);
void sleep(double seconds);
void sleep(double seconds, volatile bool* condition);
bool create_directories(const std::string& path);
void rename(const std::string& old_path, const std::string& new_path);
bool exists(const std::string& path);
std::vector<std::string> get_files(const std::string& dir, const std::string& ext = "");
std::vector<std::string> get_directories(const std::string& dir);
std::string file_name(const std::string& path);

template <typename... Types> std::string make_path(Types... arg)
{
  std::vector<std::string> v({ arg... });

  return make_path_internal(v);
}

}

#endif