#ifndef _BASE_SYSTEM_H
#define _BASE_SYSTEM_H

#include <string>
#include <stdint.h>

namespace base_system {

    uint64_t high_res_time();
    bool file_exists(const std::string& path);
    void copy_file(const std::string& src, const std::string& dest);
};

#endif