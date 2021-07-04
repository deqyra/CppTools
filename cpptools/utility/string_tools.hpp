#ifndef CPPTOOLS__UTILITY__STRING_TOOLS_HPP
#define CPPTOOLS__UTILITY__STRING_TOOLS_HPP

#include <string>
#include <vector>
#include <limits>

namespace std
{
    std::string to_string(std::string val);
}// namespace std

namespace cpptools
{
namespace String
{

// Removes any given single trailing character at the end of a string. Returns whether operation was successful.
bool popChar(std::string& str, const char c);

// Removes any given trailing substring at the end of a string. Returns whether operation was successful.
bool popString(std::string& str, const std::string& sub);

// Removes any single trailing '\r' at the end of a string. Returns whether operation was successful.
bool popCR(std::string& str);

// Removes any single trailing '\n' at the end of a string. Returns whether operation was successful.
bool popLF(std::string& str);

// Removes any trailing '\r\n' at the end of a string. Returns whether operation was successful.
bool popCRLF(std::string& str);

// Checks whether a string contains a certain substring a certain amount of times.
bool stringContains(const std::string& str, const char sub, int n = 1, bool exact = false);

// Checks whether a string is only made of digits.
bool stringIsNum(const std::string& str, bool acceptMinusSign = true);

// Checks whether a string is only made of whitespaces.
bool stringIsWhitespace(const std::string& str);

// Splits a string into substring based on a given delimiter, and returns all substrings in a vector.
std::vector<std::string> tokenizeString(const std::string& str, char delimiter, bool discardEmptyTokens = false);

// Parses an int sequence from a string and returns it in a vector.
std::vector<int> stringToIntVector(const std::string& str, char delimiter = ' ');

// Open the file located at the provided path and returns a string filled with all of its contents.
std::string readFileIntoString(const std::string& path, bool stripCarriageReturns = true);

// Takes in two multi-line strings, concatenates them line by line and returns the result in one string.
std::string multilineConcatenation(const std::string& first, const std::string& second);

// Remove carriage returns in provided string. Returns whether operation was successful.
void stripCR(std::string& str);

// Considering the input string is C / C++ source code, remove all comments from it, leaving it with only relevant source code.
void stripComments(std::string& str);

// Remove whitespace from both ends of the string.
void trim(std::string& str);

// Parse two ints separated by a single character
void parseIntRange(const std::string& input,                        // The input string to parse
                    char delimiter,                                  // The delimiter in the string
                    int& lowBound,                                   // The lower bound to be found
                    int& highBound,                                  // The higher bound to be found
                    int min = std::numeric_limits<int>::min(),       // The minimum lower bound value
                    int max = std::numeric_limits<int>::max(),       // The maximum higher bound value
                    bool throwOnExceedingBounds = false);            // Whether to throw if found bounds exceed min/max values

// Returns a string representation of the contents of an iterable.
template<typename Iter>
std::string iterableToString(
    Iter first, Iter last,                 const std::string& delimiter = " ",
    const std::string& opening = "",       const std::string& closing = "",
    const std::string& elementPrefix = "", const std::string& elementSuffix = "")
{
    std::string s = opening;
    using std::to_string;

    for (auto it = first; it != last; it++)
    {
        s += elementPrefix + to_string(*it) + elementSuffix;
        if (it != last - 1)
        {
            s += delimiter;
        }
    }

    s += closing;

    // Output example:
    // vectorToString({5, 2, 1}, "-", "<", ">", "[", "]") == "<[5]-[2]-[1]>"

    return s;
}

template<typename Iterable>
std::string iterableToString(
    const Iterable& container,             const std::string& delimiter = " ",
    const std::string& opening = "",       const std::string& closing = "",
    const std::string& elementPrefix = "", const std::string& elementSuffix = "")
{
    return iterableToString(std::begin(container), std::end(container), delimiter, opening, closing, elementPrefix, elementSuffix);
}

}// namespace String
}//namespace cpptools

#endif//CPPTOOLS__UTILITY__STRING_TOOLS_HPP