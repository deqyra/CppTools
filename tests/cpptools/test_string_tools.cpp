#include <catch2/catch2.hpp>

#include <string>

#include <cpptools/utility/string_tools.hpp>

#define TAGS "[string]"

namespace tools
{
namespace String
{

TEST_CASE("pop_char", TAGS)
{
    SECTION("End of string chars")
    {
        std::string str = "azerty\r";
        std::string res = "azerty";
        std::string res2 = "azert";

        REQUIRE(pop_char(str, '\r'));
        REQUIRE(str == res);
        REQUIRE(pop_char(str, 'y'));
        REQUIRE(str == res2);
    }

    SECTION("Mid-string chars")
    {
        std::string str = "azerty\r";
        std::string res = str;

        REQUIRE_FALSE(pop_char(str, 'z'));
        REQUIRE(str == res);
        REQUIRE_FALSE(pop_char(str, 'y'));
        REQUIRE(str == res);
    }

    SECTION("Empty string")
    {
        std::string str = "";

        REQUIRE_FALSE(pop_char(str, 'a'));
        REQUIRE(str == "");
    }
}

TEST_CASE("pop_string", TAGS)
{
    std::string str = "azerty\r";

    SECTION("End of string substrings")
    {
        std::string res = "azert";
        std::string res2 = "aze";
        std::string res3 = "az";

        REQUIRE(pop_string(str, "y\r"));
        REQUIRE(str == res);
        REQUIRE(pop_string(str, "rt"));
        REQUIRE(str == res2);
        REQUIRE(pop_string(str, "e"));
        REQUIRE(str == res3);
    }

    SECTION("Mid-string substrings")
    {
        std::string res = str;

        REQUIRE_FALSE(pop_string(str, "ze"));
        REQUIRE(str == res);
        REQUIRE_FALSE(pop_string(str, "az"));
        REQUIRE(str == res);
    }

    SECTION("Same strings provided")
    {
        std::string empty = "";
        std::string str2 = "azerty";

        REQUIRE(pop_string(empty, ""));
        REQUIRE(empty == "");
        REQUIRE(pop_string(str2, str2));
        REQUIRE(str2 == "");
    }

    SECTION("Non-present substring")
    {
        std::string res = str;

        REQUIRE_FALSE(pop_string(str, "qsd"));
        REQUIRE(str == res);
        REQUIRE_FALSE(pop_string(str, "azertyuiop"));
        REQUIRE(str == res);
    }
}

TEST_CASE("string_contains", TAGS)
{
    SECTION("General case")
    {
        std::string str = "azertyyy";

        // Inexact count (at least n occurrences)
        REQUIRE(string_contains(str, 'y', 0, false));
        REQUIRE(string_contains(str, 'y', 1, false));
        REQUIRE(string_contains(str, 'y', 2, false));
        REQUIRE(string_contains(str, 'y', 3, false));
        REQUIRE_FALSE(string_contains(str, 'y', 4, false));

        // Exact count (exactly n occurrences)
        REQUIRE_FALSE(string_contains(str, 'y', 0, true));
        REQUIRE_FALSE(string_contains(str, 'y', 1, true));
        REQUIRE_FALSE(string_contains(str, 'y', 2, true));
        REQUIRE(string_contains(str, 'y', 3, true));
        REQUIRE_FALSE(string_contains(str, 'y', 4, true));
    }

    SECTION("Empty string")
    {
        std::string str = "";

        // Searching for 0 occurrences returns true
        REQUIRE(string_contains(str, 'y', 0, false));
        REQUIRE(string_contains(str, 'y', 0, true));

        // Searching for more than 0 occurrences returns false
        REQUIRE_FALSE(string_contains(str, 'y', 1, false));
        REQUIRE_FALSE(string_contains(str, 'y', 1, true));
    }

    SECTION("0-occurrence search")
    {
        std::string str = "azerty";

        // Inexact search always returns true, no matter whether the queried character is contained in the string
        REQUIRE(string_contains(str, 'y', 0, false));
        REQUIRE(string_contains(str, 'w', 0, false));

        // Exact search returns true only if the queried character is not present in the string
        REQUIRE_FALSE(string_contains(str, 'y', 0, true));
        REQUIRE(string_contains(str, 'w', 0, true));
    }
}

TEST_CASE("string_is_num", TAGS)
{
    SECTION("General case")
    {
        std::string numeric = "3216548";
        std::string negative = "-3216548";
        std::string alphanum = "321aze456";
        std::string alphabetic = "azeqsdwxc";

        REQUIRE(string_is_num(numeric, true));
        REQUIRE(string_is_num(negative, true));
        REQUIRE_FALSE(string_is_num(alphanum, true));
        REQUIRE_FALSE(string_is_num(alphabetic, true));

        REQUIRE(string_is_num(numeric, false));
        REQUIRE_FALSE(string_is_num(negative, false));
        REQUIRE_FALSE(string_is_num(alphanum, false));
        REQUIRE_FALSE(string_is_num(alphabetic, false));
    }

    SECTION("Empty string")
    {
        REQUIRE_FALSE(string_is_num(""));
    }
}

TEST_CASE("String tokenization", TAGS)
{
    std::string str = "Hello, world. Bleeep bloop, am robot.";

    SECTION("Tokenizing on spaces")
    {
        std::vector<std::string> space_tokens = {"Hello,", "world.", "Bleeep", "bloop,", "am", "robot."};
        REQUIRE(tokenize_string(str, ' ', false) == space_tokens);

        // Discarding empty tokens makes no difference.
        REQUIRE(tokenize_string(str, ' ', true) == space_tokens);
    }

    SECTION("Tokenizing on commas")
    {
        std::vector<std::string> comma_tokens = {"Hello", " world. Bleeep bloop", " am robot."};
        REQUIRE(tokenize_string(str, ',', false) == comma_tokens);
    }

    SECTION("Tokenizing on 'e'")
    {
        std::vector<std::string> e_tokens = {"H", "llo, world. Bl", "", "", "p bloop, am robot."};
        REQUIRE(tokenize_string(str, 'e', false) == e_tokens);

        // Discarding empty tokens works.
        e_tokens = {"H", "llo, world. Bl", "p bloop, am robot."};
        REQUIRE(tokenize_string(str, 'e', true) == e_tokens);
    }

    SECTION("String ending with delimiter")
    {
        std::vector<std::string> dot_tokens = {"Hello, world", " Bleeep bloop, am robot", ""};
        REQUIRE(tokenize_string(str, '.', false) == dot_tokens);
    }

    SECTION("String starting with delimiter")
    {
        std::vector<std::string> h_tokens = {"", "ello, world. Bleeep bloop, am robot."};
        REQUIRE(tokenize_string(str, 'H', false) == h_tokens);
    }

    SECTION("Stringconsisting of delimiter")
    {
        std::vector<std::string> h_tokens = {"", ""};
        REQUIRE(tokenize_string("a", 'a', false) == h_tokens);
    }
}

TEST_CASE("Strings parsing to int vector", TAGS)
{
    SECTION("Well-formed string")
    {
        std::string str = "5 2 1";

        std::vector<int> expected = {5, 2, 1};
        REQUIRE(string_to_int_vector(str, ' ') == expected);
    }

    SECTION("String with too many delimiters")
    {
        std::string str = "     5   2        1      ";

        std::vector<int> expected = {5, 2, 1};
        REQUIRE(string_to_int_vector(str, ' ') == expected);
    }

    SECTION("Ill-formed string")
    {
        std::string str = "5;2;1";

        REQUIRE_THROWS_AS(string_to_int_vector(str, ' '), std::invalid_argument);
    }
}

TEST_CASE("read_file_into_string")
{
    std::string expected_with_carriage = "azeazeaze\r\naaaa\r\ndddd\r\n\r\ntesttest\r\n";
    std::string expected_no_carriage = "azeazeaze\naaaa\ndddd\n\ntesttest\n";

    std::string result;
    REQUIRE_NOTHROW(result = read_file_into_string("resources/tests/string/dummy_file.txt"));

    if (string_contains(result, '\r'))
    {
        REQUIRE(result == expected_with_carriage);
    }
    else
    {
        REQUIRE(result == expected_no_carriage);
    }
}

TEMPLATE_TEST_CASE("Vectors into custom string format", TAGS, int, char, std::string)
{
    SECTION("Non-empty vector")
    {
        test_type val = test_type();
        std::vector<test_type> vec = {val, val};

        // No surrounding characters
        std::string s;
        s += std::to_string(val) + " " + std::to_string(val);
        REQUIRE(String::iterable_to_string(vec) == s);

        // Single surrounding characters
        s = "<[" + std::to_string(val) + "]-[" + std::to_string(val) + "]>";
        REQUIRE(String::iterable_to_string(vec, "-", "<", ">", "[", "]") == s);

        // Surrounding strings
        s = "<~(['" + std::to_string(val) + "'] # ['" + std::to_string(val) + "'])~>";
        REQUIRE(String::iterable_to_string(vec, " # ", "<~(", ")~>", "['", "']") == s);
    }

    SECTION("Empty vector")
    {
        std::vector<test_type> vec;

        // Surrounding strings
        std::string s = "<~()~>";

        REQUIRE(String::iterable_to_string(vec, " # ", "<~(", ")~>", "['", "']") == s);
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

        REQUIRE(multiline_concatenation(str1, str2) == res);
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
