#include <catch2/catch2.hpp>

#include <string>

#include <cpptools/utility/string_tools.hpp>

#define TAGS "[string]"

namespace tools
{
namespace String
{

TEST_CASE("popChar", TAGS)
{
    SECTION("End of string chars")
    {
        std::string str = "azerty\r";
        std::string res = "azerty";
        std::string res2 = "azert";

        REQUIRE(popChar(str, '\r'));
        REQUIRE(str == res);
        REQUIRE(popChar(str, 'y'));
        REQUIRE(str == res2);
    }

    SECTION("Mid-string chars")
    {
        std::string str = "azerty\r";
        std::string res = str;

        REQUIRE_FALSE(popChar(str, 'z'));
        REQUIRE(str == res);
        REQUIRE_FALSE(popChar(str, 'y'));
        REQUIRE(str == res);
    }

    SECTION("Empty string")
    {
        std::string str = "";

        REQUIRE_FALSE(popChar(str, 'a'));
        REQUIRE(str == "");
    }
}

TEST_CASE("popString", TAGS)
{
    std::string str = "azerty\r";

    SECTION("End of string substrings")
    {
        std::string res = "azert";
        std::string res2 = "aze";
        std::string res3 = "az";

        REQUIRE(popString(str, "y\r"));
        REQUIRE(str == res);
        REQUIRE(popString(str, "rt"));
        REQUIRE(str == res2);
        REQUIRE(popString(str, "e"));
        REQUIRE(str == res3);
    }

    SECTION("Mid-string substrings")
    {
        std::string res = str;

        REQUIRE_FALSE(popString(str, "ze"));
        REQUIRE(str == res);
        REQUIRE_FALSE(popString(str, "az"));
        REQUIRE(str == res);
    }

    SECTION("Same strings provided")
    {
        std::string empty = "";
        std::string str2 = "azerty";

        REQUIRE(popString(empty, ""));
        REQUIRE(empty == "");
        REQUIRE(popString(str2, str2));
        REQUIRE(str2 == "");
    }

    SECTION("Non-present substring")
    {
        std::string res = str;

        REQUIRE_FALSE(popString(str, "qsd"));
        REQUIRE(str == res);
        REQUIRE_FALSE(popString(str, "azertyuiop"));
        REQUIRE(str == res);
    }
}

TEST_CASE("stringContains", TAGS)
{
    SECTION("General case")
    {
        std::string str = "azertyyy";

        // Inexact count (at least n occurrences)
        REQUIRE(stringContains(str, 'y', 0, false));
        REQUIRE(stringContains(str, 'y', 1, false));
        REQUIRE(stringContains(str, 'y', 2, false));
        REQUIRE(stringContains(str, 'y', 3, false));
        REQUIRE_FALSE(stringContains(str, 'y', 4, false));

        // Exact count (exactly n occurrences)
        REQUIRE_FALSE(stringContains(str, 'y', 0, true));
        REQUIRE_FALSE(stringContains(str, 'y', 1, true));
        REQUIRE_FALSE(stringContains(str, 'y', 2, true));
        REQUIRE(stringContains(str, 'y', 3, true));
        REQUIRE_FALSE(stringContains(str, 'y', 4, true));
    }

    SECTION("Empty string")
    {
        std::string str = "";

        // Searching for 0 occurrences returns true
        REQUIRE(stringContains(str, 'y', 0, false));
        REQUIRE(stringContains(str, 'y', 0, true));

        // Searching for more than 0 occurrences returns false
        REQUIRE_FALSE(stringContains(str, 'y', 1, false));
        REQUIRE_FALSE(stringContains(str, 'y', 1, true));
    }

    SECTION("0-occurrence search")
    {
        std::string str = "azerty";

        // Inexact search always returns true, no matter whether the queried character is contained in the string
        REQUIRE(stringContains(str, 'y', 0, false));
        REQUIRE(stringContains(str, 'w', 0, false));

        // Exact search returns true only if the queried character is not present in the string
        REQUIRE_FALSE(stringContains(str, 'y', 0, true));
        REQUIRE(stringContains(str, 'w', 0, true));
    }
}

TEST_CASE("stringIsNum", TAGS)
{
    SECTION("General case")
    {
        std::string numeric = "3216548";
        std::string negative = "-3216548";
        std::string alphanum = "321aze456";
        std::string alphabetic = "azeqsdwxc";

        REQUIRE(stringIsNum(numeric, true));
        REQUIRE(stringIsNum(negative, true));
        REQUIRE_FALSE(stringIsNum(alphanum, true));
        REQUIRE_FALSE(stringIsNum(alphabetic, true));

        REQUIRE(stringIsNum(numeric, false));
        REQUIRE_FALSE(stringIsNum(negative, false));
        REQUIRE_FALSE(stringIsNum(alphanum, false));
        REQUIRE_FALSE(stringIsNum(alphabetic, false));
    }

    SECTION("Empty string")
    {
        REQUIRE_FALSE(stringIsNum(""));
    }
}

TEST_CASE("String tokenization", TAGS)
{
    std::string str = "Hello, world. Bleeep bloop, am robot.";

    SECTION("Tokenizing on spaces")
    {
        std::vector<std::string> spaceTokens = {"Hello,", "world.", "Bleeep", "bloop,", "am", "robot."};
        REQUIRE(tokenizeString(str, ' ', false) == spaceTokens);

        // Discarding empty tokens makes no difference.
        REQUIRE(tokenizeString(str, ' ', true) == spaceTokens);
    }

    SECTION("Tokenizing on commas")
    {
        std::vector<std::string> commaTokens = {"Hello", " world. Bleeep bloop", " am robot."};
        REQUIRE(tokenizeString(str, ',', false) == commaTokens);
    }

    SECTION("Tokenizing on 'e'")
    {
        std::vector<std::string> eTokens = {"H", "llo, world. Bl", "", "", "p bloop, am robot."};
        REQUIRE(tokenizeString(str, 'e', false) == eTokens);
        
        // Discarding empty tokens works.
        eTokens = {"H", "llo, world. Bl", "p bloop, am robot."};
        REQUIRE(tokenizeString(str, 'e', true) == eTokens);
    }

    SECTION("String ending with delimiter")
    {
        std::vector<std::string> dotTokens = {"Hello, world", " Bleeep bloop, am robot", ""};
        REQUIRE(tokenizeString(str, '.', false) == dotTokens);
    }

    SECTION("String starting with delimiter")
    {
        std::vector<std::string> hTokens = {"", "ello, world. Bleeep bloop, am robot."};
        REQUIRE(tokenizeString(str, 'H', false) == hTokens);
    }

    SECTION("Stringconsisting of delimiter")
    {
        std::vector<std::string> hTokens = {"", ""};
        REQUIRE(tokenizeString("a", 'a', false) == hTokens);
    }
}

TEST_CASE("Strings parsing to int vector", TAGS)
{
    SECTION("Well-formed string")
    {
        std::string str = "5 2 1";

        std::vector<int> expected = {5, 2, 1};
        REQUIRE(stringToIntVector(str, ' ') == expected);
    }

    SECTION("String with too many delimiters")
    {
        std::string str = "     5   2        1      ";

        std::vector<int> expected = {5, 2, 1};
        REQUIRE(stringToIntVector(str, ' ') == expected);
    }

    SECTION("Ill-formed string")
    {
        std::string str = "5;2;1";

        REQUIRE_THROWS_AS(stringToIntVector(str, ' '), std::invalid_argument);
    }
}

TEST_CASE("readFileIntoString")
{
    std::string expectedWithCarriage = "azeazeaze\r\naaaa\r\ndddd\r\n\r\ntesttest\r\n";
    std::string expectedNoCarriage = "azeazeaze\naaaa\ndddd\n\ntesttest\n";

    std::string result;
    REQUIRE_NOTHROW(result = readFileIntoString("resources/tests/string/dummy_file.txt"));

    if (stringContains(result, '\r'))
    {
        REQUIRE(result == expectedWithCarriage);
    }
    else
    {
        REQUIRE(result == expectedNoCarriage);
    }
}

TEMPLATE_TEST_CASE("Vectors into custom string format", TAGS, int, char, std::string)
{
    SECTION("Non-empty vector")
    {
        TestType val = TestType();
        std::vector<TestType> vec = {val, val};

        // No surrounding characters
        std::string s;
        s += std::to_string(val) + " " + std::to_string(val);
        REQUIRE(String::iterableToString(vec) == s);

        // Single surrounding characters
        s = "<[" + std::to_string(val) + "]-[" + std::to_string(val) + "]>";
        REQUIRE(String::iterableToString(vec, "-", "<", ">", "[", "]") == s);

        // Surrounding strings
        s = "<~(['" + std::to_string(val) + "'] # ['" + std::to_string(val) + "'])~>";
        REQUIRE(String::iterableToString(vec, " # ", "<~(", ")~>", "['", "']") == s);
    }

    SECTION("Empty vector")
    {
        std::vector<TestType> vec;

        // Surrounding strings
        std::string s = "<~()~>";

        REQUIRE(String::iterableToString(vec, " # ", "<~(", ")~>", "['", "']") == s);
    }
}

TEST_CASE("Multiline string concatenation works properly", TAGS)
{
    SECTION("Multiline strings with equal numbers of lines")
    {
        std::string str1 =  "String 1 part 1 \n"
                            "String 1 part 2 \n"
                            "String 1 part 3 ";

        std::string str2 =  "String 2 part 1\n"
                            "String 2 part 2\n"
                            "String 2 part 3";

        std::string res =   "String 1 part 1 String 2 part 1\n"
                            "String 1 part 2 String 2 part 2\n"
                            "String 1 part 3 String 2 part 3\n";

        REQUIRE(multilineConcatenation(str1, str2) == res);
    }
}

TEST_CASE("stripCR", TAGS)
{
    // Stripping all CRs effectively removes all CRs
    std::string test = "azeazeaze\r\naaaa\r\r\r\ndddd\r\n\r\ntesttest\r\n";
    std::string expected = "azeazeaze\naaaa\ndddd\n\ntesttest\n";
    REQUIRE_NOTHROW(stripCR(test));
    REQUIRE(test == expected);
}

} // namespace String
} // namespace tools
