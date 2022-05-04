#ifndef _UTIL_H
#define _UTIL_H

#include <vector>
#include <stdint.h>
#include <string>

namespace util {
double get_time();
std::string get_date_time(const std::string& format);

bool is_pow2(size_t value);
int parse_int(const std::string& s);
uint64_t parse_uint64(const std::string& s);
std::string format(double value, int places);
std::string format(float value, int places);
std::vector<std::string> split(const std::string& str, const std::string& delim);
std::vector<uint8_t> parse_hex(const std::string& s, int count = -1);
std::vector<uint32_t> parse_hex_int(const std::string& s);
std::vector<uint64_t> parse_hex_int64(const std::string& s, int count = -1);

void get_random_bytes(void* buf, size_t len);

std::string to_hex(uint64_t);
std::string to_hex(char);
std::string to_hex(const uint8_t *ptr, size_t count);
std::string to_hex(const std::string& s);
std::string to_lower(const std::string& s);
std::string pad_string(const std::string& s, int len);

float parse_float(const std::string& s);

void get_random_bytes(unsigned char* buf, size_t len);

uint32_t endian(uint32_t x);

class Timer {

private:
    double _start = 0.0;

    double _end = 0.0;

    bool _running = false;

public:
    Timer();

    void start();
    void stop();
    double elapsed();
};

bool parse_percent(const std::string& s, double* value);

bool parse_bytes(const std::string& s, double* value);

};

#endif