#pragma once

#include <vector>
#include <string>
#include <cstdarg>
#include <cstring>

namespace Path {

    // Format a string using some arguments
    std::string format_str(const std::string&, ...);

    // Get the basename of a given path
    std::string get_basename(const std::string&);

    // Join two path to generate a new one
    std::string join_path(const std::string&, const std::string&);
};
