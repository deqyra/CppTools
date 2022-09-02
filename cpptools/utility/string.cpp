#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <limits>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

#include "string.hpp"

#include <cpptools/exception/exception.hpp>
#include <cpptools/exception/io_exception.hpp>

namespace tools::string
{

void pop_char(std::string& str, char c)
{
    if (str.size() == 0) return;

    if (str.back() == c)
    {
        str.pop_back();
    }
}

void pop_string(std::string& str, const std::string& sub)
{
    if (str.size() < sub.size()) return;

    auto pos = str.size() - sub.size();
    if (str.substr(pos, sub.size()) == sub)
    {
        str = str.substr(0, pos);
    }
}

void pop_cr(std::string& str)
{
    if (str.back() == '\r') str.pop_back();
}

void pop_lf(std::string& str)
{
    if (str.back() == '\n') str.pop_back();
}

void pop_crlf(std::string& str)
{
    if (str.back() == '\n') str.pop_back();
    if (str.back() == '\r') str.pop_back();
}

void strip_cr(std::string& str)
{
    std::size_t pos = 0;
    while ((pos = str.find('\r'), pos) != std::string::npos)
    {
        str.erase(pos);
    }
}

void strip_c_comments(std::string& str)
{
    const auto npos = std::string::npos;

    std::vector<std::pair<std::size_t, std::size_t>> comment_ranges;

    // find all comments
    std::size_t pos = str.find('/');
    while (pos != npos)
    {
        if (pos + 1 == str.size()) break;

        if (str.at(pos + 1) == '/') // found "//"
        {
            // find eol
            auto end_pos = str.find('\n', pos + 2);
            if (end_pos != npos) ++end_pos;

            comment_ranges.push_back(std::make_pair(pos, end_pos - pos));
            pos = end_pos + 1;
        }
        else if (str.at(pos + 1) == '*') // found "/*"
        {
            // find "*/"
            auto end_pos = str.find('*', pos + 2);
            if (end_pos != npos) ++end_pos;
            if (end_pos == npos || str.at(end_pos) == '/')
            {
                ++end_pos;
                comment_ranges.push_back(std::make_pair(pos, end_pos - pos));
            }
            pos = end_pos;
        }

        pos = str.find('/', pos);
    }

    // remove all comments in reverse order
    for (auto [pos, count] : comment_ranges | std::views::reverse)
    {
        str.erase(pos, count);
    }
}

bool contains(const std::string& str, char sub, std::size_t n, bool exact)
{
    if (str.size() == 0) return n == 0;
    if (!exact && n == 0) return true;

    // n° of found occurrences
    int i = 0;
    // cursor
    std::size_t found = -1;

    if (exact)
    {
        while ((found = str.find(sub, found + 1)) != std::string::npos)
        {
            i++;

            if (i > n) return false;
        }
        return (i == n);
    }
    else // if (!exact)
    {
        while ((found = str.find(sub, found + 1)) != std::string::npos)
        {
            i++;

            if (i == n) return true;
        }
        return false;
    }
}

bool is_integer(const std::string& str, bool accept_minus)
{
    if (str.size() == 0) return false;

    auto it = str.cbegin();

    if (accept_minus)
    {
        if (!std::isdigit(*it) && (*it) != '-') return false;
        ++it;
    }

    for (; it != str.cend(); it++)
    {
        if (!std::isdigit(*it)) return false;
    }
    return true;
}

bool is_whitespace(const std::string& str)
{
    for (auto it = str.cbegin(); it != str.cend(); ++it)
    {
        if (!std::isspace(*it)) return false;
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
    const std::string& str,
    char delimiter,
    bool discard_empty
)
{
    if (!contains(str, delimiter))
    {
        if (str == "" && discard_empty) return {};

        return {str};
    }

    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);

    while (std::getline(token_stream, token, delimiter))
    {
        if (discard_empty && token == "") continue;

        tokens.push_back(token);
    }

    if (str.back() == delimiter && !discard_empty)
    {
        // If required, add empty token if the string ends with the delimiter
        tokens.push_back("");
    }

    return tokens;
}

std::vector<std::size_t> parse_integer_sequence(const std::string& str, char delimiter, non_integer_action action)
{
    std::vector<std::size_t> ints;
    std::vector<std::string> tokens = tokenize(str, delimiter);

    for (auto it = tokens.cbegin(); it != tokens.cend(); it++)
    {
        std::size_t i = 0;

        if (!is_integer(*it))
        {
            switch (action)
            {
                case non_integer_action::drop:
                    continue;

                case non_integer_action::zero:
                    break;

                case non_integer_action::except:
                    auto n = (it - tokens.begin());
                    throw std::invalid_argument("Substring" + std::to_string(n) + " is not an integer.");
            }
        }
        else
        {
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
    if (f)
    {
        // Get the whole file buffer into a stringstream.
        std::stringstream s;
        s << f.rdbuf();
        f.close();

        // Process the string if required and return it.
        std::string res = s.str();
        if (strip_cr)
        {
            string::strip_cr(res);
        }

        return res;
    }

    CPPTOOLS_THROW(exception::io::file_not_found_error, path);
}

std::string multiline_concatenate(const std::string& first, const std::string& second)
{
    // split along new lines
    std::vector<std::string> first_tokens = tokenize(first, '\n');
    std::vector<std::string> second_tokens = tokenize(second, '\n');

    // strip cr
    for (auto& str : first_tokens)
    {
        pop_cr(str);
    }

    for (auto& str : second_tokens)
    {
        pop_cr(str);
    }

    auto it_first = first_tokens.cbegin();
    auto it_second = second_tokens.cbegin();

    bool first_end = it_first == first_tokens.cend();
    bool second_end = it_second == second_tokens.cend();

    std::string result;

    // join lines
    while (!first_end && !second_end)
    {
        if (!first_end)
        {
            result += *(it_first++);
            first_end = (it_first == first_tokens.cend());
        }

        if (!second_end)
        {
            result += *(it_second++);
            second_end = (it_second == second_tokens.cend());
        }

        result += '\n';
    }

    // append remaining lines
    if (!first_end)
    {
        while (it_first != first_tokens.cend())
            result += *(it_first++) + '\n';
    }
    // vvv these ^^^ are mutually exclusive
    if (!second_end)
    {
        while (it_second != second_tokens.cend())
            result += *(it_second++) + '\n';
    }

    pop_lf(result);

    return result;
}

} // namespace tools::string
