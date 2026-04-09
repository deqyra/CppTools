#include "widen_narrow.hpp"

#include <ranges>

#include <cpptools/exception/internal_exception.hpp>

namespace tools {

std::string narrow(std::wstring_view wstr) {
    auto s = std::mbstate_t{};
    auto src = wstr.data();
    
    std::size_t size;
    auto status = wcsrtombs_s(&size, nullptr, 0, &src, 0, &s);
    if (status != 0) {
        CPPTOOLS_THROW(exception::internal::unexpected_error).with_message("wcsrtombs_s failed on char count retrieval, returned " + std::to_string(status));
    }
    ++size; // null terminator

    std::string result;
    result.reserve(size);
    status = wcsrtombs_s(&size, result.data(), size, &src, size, &s);
    if (status != 0) {
        CPPTOOLS_THROW(exception::internal::unexpected_error).with_message("wcsrtombs_s failed on char conversion, returned " + std::to_string(status));
    }

    return result;
}

std::wstring widen(std::string_view str) {
    auto widened = str | std::views::transform([](char c){ return static_cast<wchar_t>(c); });
    return std::wstring( widened.begin(), widened.end() );
}

}