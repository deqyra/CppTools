#ifndef TOOLS__UTILITY__STRING_HPP
#define TOOLS__UTILITY__STRING_HPP

#include <algorithm>
#include <filesystem>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "concepts.hpp"

namespace tools::string
{

/// @brief Remove any given single trailing character at the end of a string
///
/// @param str String to process
/// @param c Char to pop off the string
void pop_char(std::string& str, char c);

/// @brief Remove any given trailing substring at the end of a string
///
/// @param str String to process
/// @param sub String to pop off str
void pop_string(std::string& str, const std::string& sub);

/// @brief Remove any single trailing '\r' at the end of a string
///
/// @param str String to process
void pop_cr(std::string& str);

/// @brief Remove any single trailing '\n' at the end of a string
///
/// @param str String to process
void pop_lf(std::string& str);

/// @brief Remove any trailing '\r\n' at the end of a string
///
/// @param str String to process
void pop_crlf(std::string& str);

/// @brief Remove all carriage return characters in the provided string
///
/// @param str String to process
void strip_cr(std::string& str);

/// @brief Remove all the content that would be considered C / C++ comments in 
/// the provided string
///
/// @param str String to process
void strip_c_comments(std::string& str);

/// @brief Check whether a string contains a certain character a certain amount
/// of times
///
/// @param str String to process
/// @param c Char to search for
/// @param n Minimal number of occurrences of c
/// @param exact Whether c must occur exactly n times in str
///
/// @return Whether str contains c as many times as described by n and exact
///
/// @note When exact is false, the function checks if c occurs at least n times
/// in str.
bool contains(
    const std::string& str,
    char c,
    std::size_t n = 1,
    bool exact = false
);

/// @brief Check whether a string is only made of digits
///
/// @param str String to process
/// @param accept_minus Whether or not to accept a single hyphen as a minus sign
bool is_integer(const std::string& str, bool accept_minus = true);

/// @brief Check whether a string is only made of whitespaces
///
/// @param str String to process
bool is_whitespace(const std::string& str);

/// @brief Remove whitespace characters from both ends of the string
///
/// @param str String to process
void trim(std::string& str);

/// @brief Split a string into substrings based on a given delimiter
///
/// @param str String to process
/// @param delimiter Char between substrings
/// @param discard_empty Whether or not to discard empty tokens
///
/// @return A vector filled with the found tokens
std::vector<std::string> tokenize(
    const std::string& str,
    char delimiter,
    bool discard_empty = false
);

/// @brief Collection of literals describing available behaviours for function
/// parse_integer_sequence upon encountering a non-integer token
enum class non_integer_action
{
    /// @brief Drop non-integer tokens
    drop,
    /// @brief Consider non-integer tokens as 0-valued
    zero,
    /// @brief Throw an exception upon encountering a non-integer token
    except
};

/// @brief Parse an integer sequence from a string
///
/// @param str String to process
/// @param delimiter Char between integers
/// @param action Literal describing the action to take upon encountering a
/// token which does not represent an integer
///
/// @return A vector filled with the found integers
std::vector<std::size_t> parse_integer_sequence(
    const std::string& str,
    char delimiter,
    non_integer_action action = non_integer_action::except
);

/// @brief Open the file located at the provided path and read its contents into
/// a string
///
/// @param path The path to the file
/// @param strip_cr Whether or not to strip carriage return characters from the
/// read input
///
/// @return A string filled with the content of the read file
std::string from_file(const std::filesystem::path& path, bool strip_cr = true);

/// @brief Take in two strings contain one or more new line characters, and
/// concatenate them line by line
///
/// @param first First multi-line string
/// @param second Second multi-line string
///
/// @return A single string containing the result of the concatenation
std::string multiline_concatenate(
    const std::string& first,
    const std::string& second
);

/// @brief Parse two integers forming the boundaries of an interval
///
/// @tparam int_t Type of integral values to parse.
///
/// @param str The string to process
/// @param delimiter The character separating the integers
///
/// @return A pair with the two parsed boundaries.
///
/// @exception If the function cannot parse an integral value as the result of
/// its processing at any point, it will throw an std::invalid_argument error.
///
/// @note The function does not care about ordering of the interval boundaries
/// in the input string. The returned pair's first member will always receive
/// the lower boundary of the read interval, and its second member will always 
/// the higher boundary.
///
/// @note If a boundary is omitted in the input string, it will be assigned
/// std::numeric_limits<int_t>::min() or max() depending on which boundary is
/// missing. Two omitted boundaries will effectively turn into the range 
/// [std::numeric_limits<int_t>::min() ; max()].
/// 
/// @note If a boundary falls outside the range of values that can be held int_t
/// when parsed, it will be clamped to the accepted range of int_t.
///
/// @note If the delimiter cannot be found in the input string, the function 
/// will attempt to parse it as a single integral value and return the interval
/// [value ; value].
template<integral_type int_t>
std::pair<int_t, int_t> parse_int_range(const std::string& str, char delimiter)
{
    static constexpr bool is_signed = std::is_signed_v<int_t>;
    static constexpr bool accept_minus = is_signed;
    static constexpr int_t min = std::numeric_limits<int_t>::min();
    static constexpr int_t max = std::numeric_limits<int_t>::max();
    static constexpr int_t width = std::numeric_limits<int_t>::digits;
    static constexpr int_t max_width = std::numeric_limits<size_t>::digits;
    
    using max_t = typename std::conditional_t<is_signed, long long int, unsigned long long int>;

    std::vector<std::string> tokens = tokenize(str, delimiter, false /* discard_empty */);

    // no delimiter
    if (tokens.size() == 1)
    {
        if (!is_integer(tokens[0], accept_minus))
        {
            std::string str = "parse_int_range: \"" + tokens[0] + "\" is not numeric and cannot be parsed.";
            throw std::invalid_argument(str.c_str());
        }

        int_t parsed = std::stoi(tokens[0]);
        return std::make_pair(parsed, parsed);
    }
    // two optional boundaries
    else if (tokens.size() == 2)
    {
        bool left_present  = (tokens[0] != "");
        bool right_present = (tokens[1] != "");

        if (!left_present && !right_present)
        {
            return std::make_pair(min, max);
        }

        if (left_present  && !is_integer(tokens[0], accept_minus))
        {
            std::string err = "parse_int_range: \"" + tokens[0] + "\" is not numeric and cannot be parsed.";
            throw std::invalid_argument(err.c_str());
        }

        if (right_present && !is_integer(tokens[1], accept_minus))
        {
            std::string err = "parse_int_range: \"" + tokens[0] + "\" is not numeric and cannot be parsed.";
            throw std::invalid_argument(err.c_str());
        }

        max_t parsed_left  = 0;
        max_t parsed_right = 0;

        if constexpr (is_signed)
        {
            if (left_present)  parsed_left  = std::stoll(tokens[0]);
            if (right_present) parsed_right = std::stoll(tokens[1]);
        }
        else
        {
            if (left_present)  parsed_left  = std::stoull(tokens[0]);
            if (right_present) parsed_right = std::stoull(tokens[1]);
        }

        int_t left  = left_present
            ? static_cast<int_t>(std::clamp(parsed_left, min, max))
            : min;

        int_t right = right_present
            ? static_cast<int_t>(std::clamp(parsed_right, min, max))
            : max;

        if (left_present && right_present && (left > right))
        {
            std::ranges::swap(left, right);
        }
        else // exactly one boundary missing
        {
            if (!left_present)  left  = min;
            if (!right_present) right = min;
        }

        return std::make_pair(left, right);
    }
    
    // too many tokens
    std::string err = "parseIntRange: Too many delimiters in \"" + str + "\".";
    throw std::invalid_argument(err.c_str());
}

/// @brief Return a string representation of the contents of a range
///
/// @param str String to process
template<std::ranges::range R>
std::string from_range(
    R range,                           const std::string& delimiter = " ",
    const std::string& prefix = "",    const std::string& suffix = "",
    const std::string& elt_prefix = "", const std::string& elt_suffix = ""
)
{
    std::string s = prefix;
    using std::to_string;
    using std::ranges::cbegin;
    using std::ranges::cend;

    auto last = cend(range);
    for (auto it = cbegin(range); it != last; it++)
    {
        s += elt_prefix + to_string(*it) + elt_suffix;
        if (it != last - 1)
        {
            s += delimiter;
        }
    }

    s += suffix;

    return s;
}

} // namespace tools::string

#endif//TOOLS__UTILITY__STRING_HPP