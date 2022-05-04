#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>

#include "util.h"

namespace {
std::random_device _r;
std::default_random_engine _rng(_r());
std::uniform_int_distribution<unsigned int> _dist;


uint8_t from_hex(char hex)
{
  if(hex >= 'a' && hex <= 'f') {
    return hex - 'a' + 10;
  } else if(hex >= 'A' && hex <= 'F') {
    return hex - 'A' + 10;
  } else if(hex >= '0' && hex <= '9') {
    return hex - '0';
  }

  throw std::invalid_argument("Invalid hex");
}

};

namespace util {

double get_time()
{
  uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  return static_cast<double>(ms) / 1000.0;
}

std::string get_date_time(const std::string& format)
{
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);

  strftime(buf, sizeof(buf), format.c_str(), &tstruct);

  return std::string(buf);
}


bool is_pow2(size_t n)
{
  if(n == 0) {
    return true;
  }

  while(n != 1) {
    if(n & 1) {
      return false;
    }
    n >>= 1;
  }

  return true;
}

uint64_t parse_uint64(const std::string& s)
{
  uint64_t value = 0;
  sscanf(s.c_str(), "%lld", &value);

  return value;
}

int parse_int(const std::string& s)
{
  int value = 0;
  sscanf(s.c_str(), "%d", &value);

  return value;
}


std::vector<uint8_t> parse_hex(const std::string& s, int count)
{
  // Pad with leading zero if necessary
  std::string hex = s;
  if(hex.length() % 2 != 0) {
    hex = "0" + hex;
  }

  std::vector<uint8_t> ara;

  for(int i = 0; i < hex.length() / 2; i++) {
    uint8_t high = from_hex(hex[2 * i]);
    uint8_t low = from_hex(hex[2 * i + 1]);

    uint8_t value = (high << 4) | low;
    ara.push_back(value);
  }

  if(count > 0 && ara.size() < count) {
    for(auto i = count - ara.size(); i > 0; i--) {
      ara.insert(ara.begin(), 0x00);
    }
  }

  return ara;
}

std::vector<uint32_t> parse_hex_int(const std::string& s)
{
  std::string hex = s;

  // Pad with leading zeros
  if(hex.length() % 8 != 0) {
    hex = std::string(8 - hex.length() % 8, '0') + hex;
  }

  std::vector<uint32_t> ara;

  for(int i = 0; i < hex.length() / 8; i++) {
    uint32_t word = 0;
    for(int j = 0; j < 4; j++) {
      word <<= 8;
      uint8_t high = from_hex(hex[8 * i + 2 * j]);
      uint8_t low = from_hex(hex[8 * i + 2 * j + 1]);

      uint8_t value = (high << 4) | low;
      word |= value;
    }
    ara.push_back(word);
  }

  return ara;
}

std::vector<uint64_t> parse_hex_int64(const std::string& s, int count)
{
  std::string hex = s;

  // Pad with leading zeros
  if(hex.length() % 16 != 0) {
    hex = std::string(8 - hex.length() % 8, '0') + hex;
  }

  if(count > 0 && hex.length() < count * 16) {
    hex = std::string(count * 16 - hex.length(), '0') + hex;
  }

  std::vector<uint64_t> ara;

  for(int i = 0; i < hex.length() / 16; i++) {
    uint64_t word = 0;
    for(int j = 0; j < 8; j++) {
      word <<= 8;
      uint8_t high = from_hex(hex[16 * i + 2 * j]);
      uint8_t low = from_hex(hex[16 * i + 2 * j + 1]);

      uint8_t value = (high << 4) | low;
      word |= value;
    }
    ara.push_back(word);
  }

  return ara;
}

std::string format(double value, int places)
{
  std::string format = std::string("%.") + std::to_string(places) + std::string("f");
  char buf[100] = { 0 };

  sprintf(buf, format.c_str(), value);

  return std::string(buf);
}

std::string format(float value, int places)
{
  return format(static_cast<double>(value), places);
}

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
  size_t start = 0;
  size_t end = str.find(delim);

  std::vector<std::string> tokens;

  while(end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + delim.length();
    end = str.find(delim, start);
  }
  if(start < str.length()) {
    tokens.push_back(str.substr(start));
  }
  return tokens;
}

void get_random_bytes(void* buf, size_t count)
{
  unsigned char* ptr = (unsigned char*)buf;

  for(int i = 0; i < count; i++) {
    ptr[i] = (unsigned char)_dist(_rng);
  }
}

std::string to_hex(uint64_t x)
{
  char hex[17] = { 0 };

  sprintf(hex, "%.16llx", x);

  return std::string(hex);
}

std::string to_hex(char c)
{
  char hex[3] = { 0 };

  sprintf(hex, "%x", (int)c);

  return std::string(hex);
}

std::string to_hex(const uint8_t* ptr, size_t count)
{
  std::string hex = "";

  for(int i = 0; i < count; i++) {
    char buf[8] = { 0 };
    uint32_t value = ptr[i];
    sprintf(buf, "%.2x", value);
    hex += std::string(buf);
  }

  return hex;
}

std::string to_hex(const std::string& s)
{
  return to_hex((const uint8_t*)s.c_str(), s.length());
}

std::string to_lower(const std::string& s)
{
  std::string s2;
  for(auto& c : s) {
    s2 += tolower(c);
  }

  return s2;
}

float parse_float(const std::string& s)
{
  float value = 0.0f;

  sscanf(s.c_str(), "%f", &value);

  return value;
}

std::string pad_string(const std::string& s, int len)
{
  if(s.length() == len) {
    return s;
  } else if(s.length() > len) {
    return s.substr(0, len);
  }

  return s + std::string(len - s.length(), ' ');
}

uint32_t endian(uint32_t x)
{
  return (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24);
}

bool parse_bytes(const std::string& s, double* value)
{
  char units[] = { 'k', 'm', 'g' };

  std::string line = to_lower(s);

  size_t idx = -1;

  for(auto& u : units) {
    if((idx = line.find(u)) != std::string::npos) {
      break;
    }
  }

  if(idx == std::string::npos) {
    return false;
  }

  std::string str = line.substr(0, idx);

  double tmp = 0.0;
  if(sscanf(str.c_str(), "%lf", &tmp) != 1) {
    return false;
  }

  switch(line[idx]) {
  case 'k':
    tmp = tmp * 1024.0;
    break;
  case 'm':
    tmp = tmp * 1024.0 * 1024.0;
    break;
  case 'g':
    tmp = tmp * 1024.0 * 1024.0 * 1024.0;
    break;
  default:
    return false;
  }

  *value = tmp;

  return true;
}

bool parse_percent(const std::string& s, double* value)
{
  size_t idx = s.find('%');

  if(idx == std::string::npos) {
    return false;
  }

  std::string sub = s.substr(0, idx);

  if(sscanf(sub.c_str(), "%lf", value) != 1) {
    return false;
  }

  return true;
}

}