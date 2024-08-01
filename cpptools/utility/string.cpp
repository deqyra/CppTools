#include <algorithm>
#include <cwchar>
#include <iostream>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

#include "string.hpp"

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/io_exception.hpp>
#include <cpptools/exception/internal_exception.hpp>

namespace tools {

void pop_char(std::string& str, char c) {
    if (str.size() == 0) { return; }

    if (str.back() == c) {
        str.pop_back();
    }
}

void pop_string(std::string& str, std::string_view sub) {
    if (str.size() < sub.size()) { return; }

    auto pos = str.size() - sub.size();
    if (str.substr(pos, sub.size()) == sub) {
        str = str.substr(0, pos);
    }
}

void pop_cr(std::string& str) {
    if (str.back() == '\r') { str.pop_back(); }
}

void pop_lf(std::string& str) {
    if (str.back() == '\n') { str.pop_back(); }
}

void pop_crlf(std::string& str) {
    if (str.back() == '\n') { str.pop_back(); }
    if (str.back() == '\r') { str.pop_back(); }
}

void strip_cr(std::string& str) {
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (*it == '\r') {
            str.erase(it.base() - 1);
        }
    }
}

void strip_c_comments(std::string& str) {
    const auto npos = std::string::npos;

    // character ranges that belong to comments, in formet {start, count}
    std::vector<std::pair<std::size_t, std::size_t>> comment_ranges;

    // find all comments
    std::size_t pos = str.find('/');
    while (pos != npos) {
        if (pos + 1 == str.size()) { break; };

        if (str.at(pos + 1) == '/') { // found "//"
            // find eol
            auto end_pos = str.find('\n', pos + 2);
            auto count = (end_pos != npos)
                ? end_pos - pos
                : npos;

            comment_ranges.push_back(std::make_pair(pos, count));
            pos = (end_pos != npos)
                ? end_pos + 1
                : npos;
        } else if (str.at(pos + 1) == '*') { // found "/*"
            // find "*/"
            auto end_pos = str.find("*/", pos + 2);
            auto count = (end_pos != npos)
                ? end_pos - pos + 2
                : npos;
            
            comment_ranges.push_back(std::make_pair(pos, count)); 
            pos = (end_pos != npos)
                ? end_pos + 3
                : npos;
        } else {
            ++pos;
        }

        pos = str.find('/', pos);
    }

    // remove all comments in reverse order
    for (auto [pos, count] : comment_ranges | std::views::reverse) {
        str.erase(pos, count);
    }
}

bool contains(std::string_view str, char sub, std::size_t n, bool exact) {
    if (str.size() == 0) { return n == 0; }
    if (!exact && n == 0) { return true; }

    // n° of found occurrences
    std::size_t i = 0;
    // cursor
    std::size_t found = -1;

    if (exact) {
        while ((found = str.find(sub, found + 1)) != std::string::npos) {
            i++;

            if (i > n) { return false; }
        }
        return (i == n);
    } else { // if (!exact)
        while ((found = str.find(sub, found + 1)) != std::string::npos) {
            i++;

            if (i == n) { return true; }
        }
        return false;
    }
}

bool is_integer(std::string_view str, bool accept_minus)
{
    if (str.size() == 0) return false;

    auto it = str.cbegin();

    if (accept_minus) {
        if (!std::isdigit(*it) && (*it) != '-') { return false; }
        ++it;
    }

    for (; it != str.cend(); it++) {
        if (!std::isdigit(*it)) { return false; }
    }
    return true;
}

bool is_whitespace(std::string_view str)
{
    for (auto it = str.cbegin(); it != str.cend(); ++it) {
        if (!std::isspace(*it)) { return false; }
    }

    return true;
}

void trim(std::string& str)
{
    static auto not_whitespace = [](unsigned char ch) {
        return !std::isspace(ch);
    };

    auto it_first = std::find_if(str.begin(), str.end(), not_whitespace);
    str.erase(str.begin(), it_first);

    auto it_last = std::find_if(str.rbegin(), str.rend(), not_whitespace).base();
    str.erase(it_last, str.end());
}

std::vector<std::string> tokenize(
    std::string_view str,
    char delimiter,
    bool discard_empty
)
{
    if (!contains(str, delimiter)) {
        if (str == "" && discard_empty) { return {}; }

        return {std::string{str}};
    }

    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(std::string{str});

    while (std::getline(token_stream, token, delimiter)) {
        if (discard_empty && token == "") continue;

        tokens.push_back(token);
    }

    if (str.back() == delimiter && !discard_empty) {
        // If required, add empty token if the string ends with the delimiter
        tokens.push_back("");
    }

    return tokens;
}

std::vector<std::size_t> parse_integer_sequence(std::string_view str, char delimiter, non_integer_action action)
{
    std::vector<std::size_t> ints;
    std::vector<std::string> tokens = tokenize(str, delimiter, /* discardEmpty */ true);

    for (auto it = tokens.cbegin(); it != tokens.cend(); it++) {
        std::size_t i = 0;

        if (!is_integer(*it)) {
            switch (action) {
            case non_integer_action::drop:
                continue;

            case non_integer_action::zero:
                break;

            case non_integer_action::exception:
                auto n = (it - tokens.begin());
                throw std::invalid_argument("Substring" + std::to_string(n) + " is not an integer.");
            }
        } else {
            i = std::stoi(*it);
        }

        ints.push_back(i);
    }

    return ints;
}

std::string from_file(const std::filesystem::path& path, bool strip_cr)
{
    // Open the file for reading.
    std::ifstream f(path.c_str(), std::ios::in);
    if (f) {
        // Get the whole file buffer into a stringstream.
        std::stringstream s;
        s << f.rdbuf();
        f.close();

        // Process the string if required and return it.
        std::string res = s.str();
        if (strip_cr) {
            ::tools::strip_cr(res);
        }

        return res;
    }

    CPPTOOLS_THROW(exception::io::file_not_found_error, path);
}

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

std::string multiline_concatenate(std::string_view first, std::string_view second)
{
    // split along new lines
    std::vector<std::string> first_tokens = tokenize(first, '\n');
    std::vector<std::string> second_tokens = tokenize(second, '\n');

    // strip cr
    for (auto& str : first_tokens) {
        pop_cr(str);
    }

    for (auto& str : second_tokens) {
        pop_cr(str);
    }

    auto it_first = first_tokens.cbegin();
    auto it_second = second_tokens.cbegin();

    bool first_end = it_first == first_tokens.cend();
    bool second_end = it_second == second_tokens.cend();

    std::string result;

    // join lines
    while (!first_end && !second_end) {
        if (!first_end) {
            result += *(it_first++);
            first_end = (it_first == first_tokens.cend());
        }

        if (!second_end) {
            result += *(it_second++);
            second_end = (it_second == second_tokens.cend());
        }

        result += '\n';
    }

    // append remaining lines
    if (!first_end) {
        while (it_first != first_tokens.cend()) {
            result += *(it_first++) + '\n';
        }
    }
    // vvv these ^^^ are mutually exclusive
    if (!second_end) {
        while (it_second != second_tokens.cend()) {
            result += *(it_second++) + '\n';
        }
    }

    pop_lf(result);

    return result;
}

} // namespace tools
