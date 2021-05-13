#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>
#include <cstdarg>
#include <cstring>

namespace Path {

    // Format a string using some arguments
    std::string format_str(const std::string&, ...);

    // Get the basename of a given path
    std::string get_basename(const std::string&);

    // Join two path to generate a new one
    std::string join_path(const std::string&, const std::string&);

    // Make all directories in a given path
    void make_path(std::string, const char sep = '/');
};
