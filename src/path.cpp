#include <path.hpp>

// Definition of the formating function
std::string Path::format_str(const std::string& fmt, ...)
{
    // Generate a list containing all arguments
    va_list args;

    // The list of optional arguments start after fmt
    va_start (args, fmt);
    
    // Get all the optional arguments
    size_t len = std::vsnprintf(NULL, 0, fmt.c_str(), args);

    va_end (args);
    std::vector<char> vec(len + 1);
    va_start (args, fmt);
    std::vsnprintf(&vec[0], len + 1, fmt.c_str(), args);
    va_end (args);

    // Return the formated string
    return &vec[0];
}

// Get the basename of a path
std::string Path::get_basename(const std::string& str)
{
    // Separator between paths
    const char sep = '/';

    size_t i = str.rfind(sep, str.length());
    if (i != std::string::npos) {
        return(str.substr(i+1, str.length() - i));
    }

    return("");
}

// Join a two paths taking into account the separators
std::string Path::join_path(const std::string& a, const std::string& b)
{
    // Get the last character in the a string
    const char final_a = a.back();
    const char begin_b = b[0];

    if (final_a != '/' && begin_b != '/') {
        return a + '/' + b;
    } else if (final_a == '/' && begin_b == '/') {
        return a.substr(0, a.size() - 1) + b;
    } else {
        return a + b;
    }
}

// Make all directories in a path if they do not exist
void Path::make_path(std::string path, const char sep)
{
    // Split the string using the separator
    std::replace(path.begin(), path.end(), sep, ' ');

    // Vector where the separated data will be stored
    std::vector<std::string> separated_path;

    // Stringstream to separate the data
    std::stringstream ss(path);

    // Push back all subdirectories in the vector
    std::string temp;
    while (ss >> temp) separated_path.push_back(temp);

    // Reconstructed path used to create all directories
    std::string reconstructed_path = "";

    // Iterate for all elements in the path
    for (auto& p : separated_path) {

        // Append the subpath to the reconstructed path
        reconstructed_path += p + sep;

        // Create the directory if it does not exist
        mkdir(reconstructed_path.c_str(), 0777);
    }
}
