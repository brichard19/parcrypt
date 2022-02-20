#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "util.h"

namespace LogLevel {
bool is_valid(int level)
{
  switch(level) {
  case Info:
  case Error:
  case Debug:
    return true;
  default:
    return false;
  }
}

std::string to_string(int level)
{
  switch(level) {
  case Info:
    return "Info";
  case Error:
    return "Error";
  case Debug:
    return "Debug";
  case Warning:
    return "Warning";
  }

  return "";
}
};


std::string Logger::format_log(int logLevel, std::string msg)
{
  std::string date_time = util::get_date_time("%X");

  std::string prefix = "[" + date_time + "] ";

  size_t prefix_len = prefix.length();

  std::string padding(prefix_len, ' ');

  if(msg.find('\n', 0) != std::string::npos) {
    size_t pos = 0;
    size_t prev = 0;

    while((pos = msg.find('\n', prev)) != std::string::npos) {
      prefix += msg.substr(prev, pos - prev) + "\n" + padding;
      prev = pos + 1;
    }

    prefix += msg.substr(prev);
  } else {
    prefix += msg;
  }

  return prefix;
}


void Logger::log(int logLevel, std::string msg)
{
  std::string str = format_log(logLevel, msg);

  fprintf(stdout, "%s\n", str.c_str());
  fflush(stdout);
}

void Logger::set_log_file(std::string path)
{

}
