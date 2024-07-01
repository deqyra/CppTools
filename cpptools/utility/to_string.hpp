#ifndef CPPTOOLS_UTILITY_TO_STRING_HPP
#define CPPTOOLS_UTILITY_TO_STRING_HPP

#include <concepts>
#include <cstdint>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <string>

#include <cpptools/api.hpp>

namespace tools
{

namespace detail::to_string
{
    CPPTOOLS_API std::string to_string(const std::source_location& location);

    template<typename T>
    std::string to_string(T* ptr)
    {
        if (ptr == nullptr) {
            return "nullptr";
        }

        std::ostringstream ss;
        ss << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(std::intptr_t) * 2) << reinterpret_cast<std::intptr_t>(ptr);

        return ss.str();
    }

    using std::to_string;

    struct to_string_cpo {
        template <typename T>
        requires requires (const T& t) {
            { to_string(t) } -> std::same_as<std::string>;
        }
        std::string operator()(const T& t) const {
            return to_string(t);
        }

        CPPTOOLS_API const std::string& operator()(const std::string& t) const {
            return t;
        }
    };
}

inline constexpr auto to_string = detail::to_string::to_string_cpo{};

} // namespace tools

#endif//CPPTOOLS_UTILITY_TO_STRING_HPP