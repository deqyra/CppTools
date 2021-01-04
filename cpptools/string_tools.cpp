#include "string_tools.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "exceptions/file_not_found_error.hpp"
#include "exceptions/range_bounds_exceeded_error.hpp"

namespace std
{
    std::string to_string(std::string val)
    {
        return val;
    }
} // namespace std

namespace CppTools
{
namespace String
{

bool popChar(std::string& str, const char c)
{
    if (str.size() == 0) return false;

    if (str.back() == c)
    {
        str.pop_back();
        return true;
    }
    return false;
}

bool popString(std::string& str, const std::string& sub)
{
    if (str == sub)
    {
        str = "";
        return true;
    }

    if (str.size() < sub.size()) return false;

    // Check whether substring is contained at the endFill of the main string.
    if (str.substr(str.size() - sub.size(), sub.size()) == sub)
    {
        str = str.substr(0, str.size() - sub.size());
        return true;
    }
    return false;
}

bool popCR(std::string& str)
{
    return popChar(str, '\r');
}

bool popLF(std::string& str)
{
    return popChar(str, '\n');
}

bool popCRLF(std::string& str)
{
    return popString(str, "\r\n");
}

bool stringContains(const std::string& str, const char sub, int n, bool exact)
{
    if (str.size() == 0) return n == 0;
    if (!exact && n == 0) return true;

    // Counter for found occurrences.
    int i = 0;
    // Kind of a string cursor.
    std::size_t found = -1;

    // Search the string while the cursor is valid.
    do
    {
        // Search only after the last found position.
        // found is initialized at -1, which gives 0 for found + 1 at the first iteration.
        found = str.find(sub, found + 1);

        if (found != std::string::npos)
        {
            // Increment count.
            i++;

            // If an inexact count was requested (_at least_ n occurrences), return true as soon as the count is met.
            if (!exact && i == n)
            {
                return true;
            }
            // If an exact count was requested (_exactly_ n occurrences), return false as soon as the count is passed.
            if (exact && i > n)
            {
                return false;
            }
        }
    } while (found != std::string::npos);

    if (i == n)
    {
        return true;
    }

    return false;
}

bool stringIsNum(const std::string& str, bool acceptMinusSign)
{
    if (str.size() == 0) return false;
    
    int index = 0;
    // Check every character in the string.
    for (auto it = str.begin(); it != str.end(); it++)
    {
        if (!acceptMinusSign)
        {
            // Return false at the first non-digit char found.
            if (!std::isdigit(*it)) return false;
        }
        else
        {
            // Return false at the first non-digit char found,
            // EXCEPT if it's a minus sign (hyphen) AND it's the first character.
            if (index == 0 && !std::isdigit(*it) && (*it) != '-') return false;
            if (index != 0 && !std::isdigit(*it)) return false;
        }

        index++;
    }
    return true;
}

bool stringIsWhitespace(const std::string& str)
{
    if (str.size() == 0) return true;

    for (auto it = str.begin(); it != str.end(); it++)
    {
        if (!std::isspace(*it)) return false;
    }

    return true;
}

std::vector<std::string> tokenizeString(const std::string& str, char delimiter, bool discardEmptyTokens)
{
    // If the string does not contain the delimiter, return the string as-is (within a vector).
    if (!stringContains(str, delimiter))
    {
        // EXCEPT if it's empty and discarding empty tokens was required.
        if (str == "" && discardEmptyTokens)
        {
            // In which case, return an empty vector.
            return {};
        }

        return {str};
    }

    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        // If specified so, empty tokens should not be added to the output vector.
        if (discardEmptyTokens && token == "") continue;
        
        tokens.push_back(token);
    }

    if (str.back() == delimiter && !discardEmptyTokens)
    {
        // Manually add an empty token if the string ends with the delimiter.
        tokens.push_back("");

        // This is to ensure consistency with the behaviour of the function when the string _starts_ with the delimiter.
        // In such a case, an empty token is automatically detected first.
        // This behaviour is also more representative of how the input string was formatted.
    }

    return tokens;
}

std::vector<int> stringToIntVector(const std::string& str, char delimiter)
{
    std::vector<int> hints;
    // Tokenise input string according to provided delimiter (true = discard any empty token).
    std::vector<std::string> tokens = tokenizeString(str, delimiter, true);

    for (auto it = tokens.begin(); it != tokens.end(); it++)
    {
        // By default, std::stoi will successfully parse any string which begins with a digit, 
        // discarding the rest of the content from the point it cannot be parsed anymore.
        //
        // From the standpoint of this function, if string "53;<>!12aaa" was an extracted token,
        // then it is clearly invalid and should not be parsed.
        // However, std::stoi("53;<>!12aaa") does not throw and returns 53.
        //
        // This may be seen as kind of a behavioural discrepancy, leading to confusion on the
        // side of the user. Therefore, it is strictly required that any token must be 
        // exclusively comprised of digits (and a minus sign if any), which is what the following condition ensures.

        if (!stringIsNum(*it))
        {
            throw std::invalid_argument("Provided string does not represent an integer.");
        }

        // Push parsed token in output vector.
        hints.push_back(std::stoi(*it));
    }

    return hints;
}

std::string readFileIntoString(const std::string& path, bool stripCarriageReturns)
{
    // Open the file for reading.
    std::ifstream f = std::ifstream(path.c_str(), std::ios::in);
    if (f)
    {
        // Get the whole file buffer into a stringstream.
        std::stringstream s;
        s << f.rdbuf();
        f.close();

        // Process the string if required and return it.
        std::string res = s.str();
        if (stripCarriageReturns)
        {
            stripCR(res);
        }

        return res;
    }

    throw FileNotFoundError(path + ": file not found.");
}

std::string multilineConcatenation(const std::string& first, const std::string& second)
{
    // Step 1: split input strings into vectors of lines.
    std::vector<std::string> firstTokens = tokenizeString(first, '\n');
    std::vector<std::string> secondTokens = tokenizeString(second, '\n');

    // Step 2: remove potential trailing '\r's from every line in both vectors (can happen on Windows).
    for (auto it = firstTokens.begin(); it != firstTokens.end(); it++)
    {
        popCR(*it);
    }
    
    for (auto it = secondTokens.begin(); it != secondTokens.end(); it++)
    {
        popCR(*it);
    }

    // Step 3: iterate on both vectors at the same time and join their lines together.
    auto itFirst = firstTokens.begin();
    auto itSecond = secondTokens.begin();

    // Loop control: stop when either vector reaches its end.
    bool stop = ((itFirst == firstTokens.end()) || (itSecond == secondTokens.end()));

    std::string s;
    while (!stop)
    {
        // Step 3.1: feed first line into stream.
        if (itFirst != firstTokens.end())
        {
            s += *itFirst;
            itFirst++;
        }

        // Step 3.2: feed second line into stream.
        if (itSecond != secondTokens.end())
        {
            s += *itSecond;
            itSecond++;
        }
        // Don't forget to move iterators forward each time a line is accessed.

        // Step 3.3: break line and check for loop end.
        s += '\n';
        stop = ((itFirst == firstTokens.end()) || (itSecond == secondTokens.end()));
    }

    // Step 4: return single string with everything.
    return s;
}

void stripCR(std::string& str)
{
    std::string result;
    std::vector<std::string> tokens = tokenizeString(str, '\r');
    for (auto it = tokens.begin(); it != tokens.end(); it++)
    {
        //popCR(*it);         // Unneeded.
        result += *it;
    }
    str = result;        
}

void stripComments(std::string& str)
{
    std::string result;

    // Find and remove multiline comments
    size_t offset = str.find("/*", 0);
    while (offset != std::string::npos)
    {
        // Find end of multiline comment
        size_t end = str.find("*/", offset);
        // Find total length of comment
        size_t len = std::string::npos;
        if (end != std::string::npos)
        {
            // Account for extra 2 chars in "*/"
            len = end - offset + 2;
        }

        // Erase all of it
        str.erase(offset, len);

        offset = str.find("/*", 0);
    }

    // Find and remove single line comments
    offset = str.find("//", 0);
    while (offset != std::string::npos)
    {
        // Find end of multiline comment
        size_t end = str.find('\n', offset);
        // Find total length of comment
        size_t len = std::string::npos;
        if (end != std::string::npos)
        {
            // Do not account for extra 1 char '\n', leave as is
            len = end - offset;
        }

        // Erase all of it
        str.erase(offset, len);

        offset = str.find("//", 0);
    }
}

void trim(std::string& str)
{
    static auto isNonWhite = [](unsigned char ch) {
        return !std::isspace(ch);
    };

    auto itFirstNonWhite = std::find_if(str.begin(), str.end(), isNonWhite);
    str.erase(str.begin(), itFirstNonWhite);

    auto itLastNonWhite = std::find_if(str.rbegin(), str.rend(), isNonWhite).base();
    str.erase(itLastNonWhite, str.end());
}

void parseIntRange(const std::string& input, char delimiter, int& lowBound, int& highBound, int min, int max, bool throwOnExceedingBounds)
{
    // Assuming ':' as delimiter, the expected format is like "-32:64".
    // ":64" and "-32:" are accepted, meaning either end is unbounded,
    // as well as a single number like "64", meaning the range consists only of that number.
    // ":" means both ends are unbounded.
    
    if (input == "")
    {
        throw std::invalid_argument("parseIntRange: cannot parse range in empty string.");
    }

    if (min > max)
    {
        std::string str = "parseIntRange: min " + std::to_string(min) + " is greater than max " + std::to_string(max) + ".";
        throw std::invalid_argument(str.c_str());
    }

    // Tokenize on delimiter.
    std::vector<std::string> tokens = tokenizeString(input, delimiter, false);

    // If only one token was extracted, then the input string had no delimiter.
    if (tokens.size() == 1)
    {
        if (!stringIsNum(tokens[0]))
        {
            std::string str = "parseIntRange: \"" + std::to_string(tokens[0]) + "\" is not numeric and cannot be parsed.";
            throw std::invalid_argument(str.c_str());
        }

        // Parse the input number.
        int parsed = std::stoi(tokens[0]);

        // If the parsed number is out of bounds...
        if (parsed < min || parsed > max)
        {
            // Throw if required.
            if (throwOnExceedingBounds)
            {
                std::string str = "parseIntRange: value " + std::to_string(parsed) + " is not between " + std::to_string(min) + " and " + std::to_string(max) + ".";
                throw RangeBoundsExceededError(str);
            }
            // Set the bounds to the appropriate values otherwise.
            else
            {
                if (parsed < min)
                {
                    lowBound = min;
                    highBound = min;
                }
                else // if (parsed > max)
                {
                    lowBound = max;
                    highBound = max;
                }
            }
        }
        else
        {
            // Otherwise just set both bounds to the input number.
            lowBound = parsed;
            highBound = parsed;
        }
    }
    // If two tokens were extracted, then the input string had a delimiter.
    else if (tokens.size() == 2)
    {
        // Empty tokens are accepted, meaning the corresponding end is unbounded.
        if ( (tokens[0] != "" && !stringIsNum(tokens[0])) || 
                (tokens[1] != "" && !stringIsNum(tokens[1])) )
        {
            std::string str = "parseIntRange: \"" + std::to_string(tokens[0]) + "\" or \"" + std::to_string(tokens[1]) + "\" are not numeric and cannot be parsed.";
            throw std::invalid_argument(str.c_str());
        }

        // If the lower bound is empty, give it the minimum value.
        if (tokens[0] == "")
        {
            lowBound = min;
        }
        else
        {
            // Else, parse it.
            lowBound = std::stoi(tokens[0]);
        }

        // If the higher bound is empty, give it the maximum value.
        if (tokens[1] == "")
        {
            highBound = max;
        }
        else
        {
            // Else, parse it.
            highBound = std::stoi(tokens[1]);
        }

        // Swap both bounds around if needed.
        if (lowBound > highBound)
        {
            std::swap(lowBound, highBound);
        }

        // If the higher bound is greater than the maximum...
        if (highBound > max)
        {
            // Throw if required.
            if (throwOnExceedingBounds)
            {
                std::string str = "parseIntRange: higher bound " + std::to_string(highBound) + " is above maximum" + std::to_string(max) + ".";
                throw RangeBoundsExceededError(str);
            }
            // Otherwise set it at the max value.
            else
            {
                highBound = max;
            }                    
        }

        // If the lower bound is lower than the minimum...
        if (lowBound < min)
        {
            // Throw if required.
            if (throwOnExceedingBounds)
            {
                std::string str = "parseIntRange: lower bound " + std::to_string(lowBound) + " is below minimum" + std::to_string(min) + ".";
                throw RangeBoundsExceededError(str);
            }
            // Otherwise set it at the min value.
            else
            {
                lowBound = min;
            }                    
        }
    }
    else // if (tokens.size() > 2)
    {
        std::string str = "parseIntRange: Too many delimiters in \"" + input + "\".";
        throw std::invalid_argument(str.c_str());
    }
}

}// namespace String
}//namespace CppTools
