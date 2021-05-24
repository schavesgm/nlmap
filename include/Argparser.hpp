#pragma once

#include <algorithm>
#include <vector>
#include <sstream>

class Argparser
{
public:
    // -- Copy the contents of the argv inside the tokens vector
    Argparser(const int& argc, char** argv)
    {
        for (int i = 1; i < argc; i++)
            this->tokens.push_back(std::string(argv[i]));
    }

    // -- Get the contents of a flag using templated arguments
    template <typename T>
    T get_flag(const std::string& flag) const
    {
        // Find the position in the vector that contains the flag
        auto pos = std::find(this->tokens.begin(), this->tokens.end(), flag);

        // Check if the flag is found and return the next value if needed
        if (pos != this->tokens.end() && ++pos != this->tokens.end()) {

            // Generate a string stream in the current position
            std::stringstream caster(*pos);

            // Cast the string to the correct type
            T casted_str = 0; caster >> casted_str;

            return casted_str;
        }

        // Return the zero value in case nothing is found
        return T(0);
    }

    // -- Template specification for std::strings
    std::string get_flag(const std::string& flag) const
    {
        // Find the position in the vector that contains the flag
        auto pos = std::find(this->tokens.begin(), this->tokens.end(), flag);

        // Check if the flag is found and return the next value if needed
        if (pos != this->tokens.end() && ++pos != this->tokens.end()) {
            return *pos;
        }

        static const std::string non_present("");;
        return non_present;
    }

    // -- Check if a flag exists in the argument parser
    bool check_flag(const std::string& flag) const
    {
        // Get the position of the flag
        const auto pos = std::find(this->tokens.begin(), this->tokens.end(), flag);
        return pos != tokens.end();
    }

    // -- Get all flags in the argument parser
    std::vector<std::string> get_flags() const
    {
        // Vector that will contain all the flags
        std::vector<std::string> flags;

        for (long unsigned int i = 0; i < this->tokens.size(); i += 2) {
            flags.push_back(this->tokens[i]);
        }

        return flags;
    }

private:
    std::vector<std::string> tokens;
};
