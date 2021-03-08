#include <utility>

namespace Format 
{
    // Use namespace std in this namespace to light the std:: overload
    using namespace std;

    // Convert to c_str if type is a std::string.
    template <typename T> 
    auto to_cstr(T&& t);

    template <typename... Args> 
    std::string format_internal(const std::string&, Args&&...);

    template <typename... Args>
    std::string format_string(const std::string&, Args&&...);
}

template <typename T>
auto Format::to_cstr(T&& t)
{
    if constexpr(is_same<remove_cv_t<remove_reference_t<T>>, string>::value) {
        return forward<T>(t).c_str();
    } else {
        return forward<T>(t);
    }
}

template <typename... Args>
std::string Format::format_internal(const std::string& fmt, Args&& ... args)
{
    // Get the size of the formatted string if args were expanded
    const auto size = snprintf(nullptr, 0, fmt.c_str(), forward<Args>(args)...) + 1;
    const auto buffer = make_unique<char[]>(size);

    // Fill the buffer with the expanded formatting
    snprintf(buffer.get(), size, fmt.c_str(), forward<Args>(args)...);

    return std::string(buffer.get(), buffer.get() + size - 1);
}


template <typename... Args>
std::string Format::format_string(const std::string& fmt, Args&&... args)
{
    return format_internal(fmt, to_cstr(std::forward<Args>(args))...);
}
