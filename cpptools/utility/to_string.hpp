#ifndef CPPTOOLS__UTILITY__TO_STRING_HPP
#define CPPTOOLS__UTILITY__TO_STRING_HPP

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <string>
#include <type_traits>

namespace tools
{

namespace detail::to_string
{
    std::string to_string(const std::source_location& location);

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

        const std::string& operator()(const std::string& t) const {
            return t;
        }
    };
}

inline constexpr auto to_string = detail::to_string::to_string_cpo{};

} // namespace tools

#endif//CPPTOOLS__UTILITY__TO_STRING_HPP