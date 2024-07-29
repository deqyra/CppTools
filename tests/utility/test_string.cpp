#include <catch2/catch_all.hpp>

#include <string>
#include <type_traits>

#include <cpptools/utility/string.hpp>
#include <cpptools/utility/to_string.hpp>

#define TAGS "[string]"

namespace tools::string {

TEST_CASE("pop_char", TAGS) {
    SECTION("End of string chars") {
        std::string str = "azerty\r";
        std::string res = "azerty";
        std::string res2 = "azert";

        pop_char(str, '\r');
        REQUIRE(str == res);

        pop_char(str, 'y');
        REQUIRE(str == res2);
    }

    SECTION("Mid-string chars") {
        std::string str = "azerty\r";
        std::string res = str;

        pop_char(str, 'z');
        REQUIRE(str == res);

        pop_char(str, 'y');
        REQUIRE(str == res);
    }

    SECTION("Empty string") {
        std::string str = "";

        pop_char(str, 'a');
        REQUIRE(str == "");
    }
}

TEST_CASE("pop_string", TAGS) {
    std::string str = "azerty\r";

    SECTION("End of string substrings") {
        std::string res = "azert";
        std::string res2 = "aze";
        std::string res3 = "az";

        pop_string(str, "y\r");
        REQUIRE(str == res);

        pop_string(str, "rt");
        REQUIRE(str == res2);

        pop_string(str, "e");
        REQUIRE(str == res3);
    }

    SECTION("Mid-string substrings") {
        std::string res = str;

        pop_string(str, "ze");
        REQUIRE(str == res);

        pop_string(str, "az");
        REQUIRE(str == res);
    }

    SECTION("Same strings provided") {
        std::string empty = "";
        std::string str2 = "azerty";

        pop_string(empty, "");
        REQUIRE(empty == "");

        pop_string(str2, str2);
        REQUIRE(str2 == "");
    }

    SECTION("Non-present substring") {
        std::string res = str;

        pop_string(str, "qsd");
        REQUIRE(str == res);

        pop_string(str, "azertyuiop");
        REQUIRE(str == res);
    }
}

TEST_CASE("string_contains", TAGS) {
    SECTION("General case") {
        std::string str = "azertyyy";

        // Inexact count (at least n occurrences)
        REQUIRE(contains(str, 'y', 0, false));
        REQUIRE(contains(str, 'y', 1, false));
        REQUIRE(contains(str, 'y', 2, false));
        REQUIRE(contains(str, 'y', 3, false));
        REQUIRE_FALSE(contains(str, 'y', 4, false));

        // Exact count (exactly n occurrences)
        REQUIRE_FALSE(contains(str, 'y', 0, true));
        REQUIRE_FALSE(contains(str, 'y', 1, true));
        REQUIRE_FALSE(contains(str, 'y', 2, true));
        REQUIRE(contains(str, 'y', 3, true));
        REQUIRE_FALSE(contains(str, 'y', 4, true));
    }

    SECTION("Empty string") {
        std::string str = "";

        // Searching for 0 occurrences returns true
        REQUIRE(contains(str, 'y', 0, false));
        REQUIRE(contains(str, 'y', 0, true));

        // Searching for more than 0 occurrences returns false
        REQUIRE_FALSE(contains(str, 'y', 1, false));
        REQUIRE_FALSE(contains(str, 'y', 1, true));
    }

    SECTION("0-occurrence search") {
        std::string str = "azerty";

        // Inexact search always returns true, no matter whether the queried character is contained in the string
        REQUIRE(contains(str, 'y', 0, false));
        REQUIRE(contains(str, 'w', 0, false));

        // Exact search returns true only if the queried character is not present in the string
        REQUIRE_FALSE(contains(str, 'y', 0, true));
        REQUIRE(contains(str, 'w', 0, true));
    }
}

TEST_CASE("string_is_num", TAGS) {
    SECTION("General case") {
        std::string numeric = "3216548";
        std::string negative = "-3216548";
        std::string alphanum = "321aze456";
        std::string alphabetic = "azeqsdwxc";

        REQUIRE(is_integer(numeric, true));
        REQUIRE(is_integer(negative, true));
        REQUIRE_FALSE(is_integer(alphanum, true));
        REQUIRE_FALSE(is_integer(alphabetic, true));

        REQUIRE(is_integer(numeric, false));
        REQUIRE_FALSE(is_integer(negative, false));
        REQUIRE_FALSE(is_integer(alphanum, false));
        REQUIRE_FALSE(is_integer(alphabetic, false));
    }

    SECTION("Empty string") {
        REQUIRE_FALSE(is_integer(""));
    }
}

TEST_CASE("String tokenization", TAGS) {
    std::string str = "Hello, world. Bleeep bloop, am robot.";

    SECTION("Tokenizing on spaces") {
        std::vector<std::string> space_tokens = {"Hello,", "world.", "Bleeep", "bloop,", "am", "robot."};
        REQUIRE(tokenize(str, ' ', false) == space_tokens);

        // Discarding empty tokens makes no difference.
        REQUIRE(tokenize(str, ' ', true) == space_tokens);
    }

    SECTION("Tokenizing on commas") {
        std::vector<std::string> comma_tokens = {"Hello", " world. Bleeep bloop", " am robot."};
        REQUIRE(tokenize(str, ',', false) == comma_tokens);
    }

    SECTION("Tokenizing on 'e'") {
        std::vector<std::string> e_tokens = {"H", "llo, world. Bl", "", "", "p bloop, am robot."};
        REQUIRE(tokenize(str, 'e', false) == e_tokens);

        // Discarding empty tokens works.
        e_tokens = {"H", "llo, world. Bl", "p bloop, am robot."};
        REQUIRE(tokenize(str, 'e', true) == e_tokens);
    }

    SECTION("String ending with delimiter") {
        std::vector<std::string> dot_tokens = {"Hello, world", " Bleeep bloop, am robot", ""};
        REQUIRE(tokenize(str, '.', false) == dot_tokens);
    }

    SECTION("String starting with delimiter") {
        std::vector<std::string> h_tokens = {"", "ello, world. Bleeep bloop, am robot."};
        REQUIRE(tokenize(str, 'H', false) == h_tokens);
    }

    SECTION("Stringconsisting of delimiter") {
        std::vector<std::string> h_tokens = {"", ""};
        REQUIRE(tokenize("a", 'a', false) == h_tokens);
    }
}

TEST_CASE("Strings parsing to int vector", TAGS) {
    SECTION("Well-formed string") {
        std::string str = "5 2 1";

        std::vector<size_t> expected = {5, 2, 1};
        REQUIRE(parse_integer_sequence(str, ' ') == expected);
    }

    SECTION("String with too many delimiters") {
        std::string str = "     5   2        1      ";

        std::vector<size_t> expected = {5, 2, 1};
        REQUIRE(parse_integer_sequence(str, ' ') == expected);
    }

    SECTION("Ill-formed string") {
        std::string str = "5;2;1";

        REQUIRE_THROWS_AS(parse_integer_sequence(str, ' '), std::invalid_argument);
    }
}

TEST_CASE("Dump a file into a string") {
    std::string expected_with_carriage = "azeazeaze\r\naaaa\r\ndddd\r\n\r\ntesttest\r\n";
    std::string expected_no_carriage = "azeazeaze\naaaa\ndddd\n\ntesttest\n";

    std::string result;
    REQUIRE_NOTHROW(result = from_file("resources/utility/string/dummy_file.txt"));

    if (contains(result, '\r')) {
        REQUIRE(result == expected_with_carriage);
    } else {
        REQUIRE(result == expected_no_carriage);
    }
}

TEMPLATE_TEST_CASE("Vectors into custom string format", TAGS, int, std::string) {
    SECTION("Non-empty vector") {
        TestType val = TestType{};
        std::vector<TestType> vec = {val, val};

        std::string s;
        std::string string_val;
        if constexpr (std::is_same_v<TestType, std::string>) {
            string_val = val;
        } else {
            string_val = tools::to_string(val);
        }

        // No surrounding characters
        s += string_val + " " + string_val;
        REQUIRE(from_range(vec) == s);

        // Single surrounding characters
        s = "<[" + string_val + "]-[" + string_val + "]>";
        REQUIRE(from_range(vec, "-", "<", ">", "[", "]") == s);

        // Surrounding strings
        s = "<~(['" + string_val + "'] # ['" + string_val + "'])~>";
        REQUIRE(from_range(vec, " # ", "<~(", ")~>", "['", "']") == s);
    }

    SECTION("Empty vector") {
        std::vector<TestType> vec;

        // Surrounding strings
        std::string s = "<~()~>";

        REQUIRE(from_range(vec, " # ", "<~(", ")~>", "['", "']") == s);
    }
}

TEST_CASE("Multiline string concatenation works properly", TAGS) {
    SECTION("Multiline strings with equal numbers of lines") {
        std::string str1 =
            "String 1 part 1 \n"
            "String 1 part 2 \n"
            "String 1 part 3 ";

        std::string str2 =
            "String 2 part 1\n"
            "String 2 part 2\n"
            "String 2 part 3";

        std::string res =
            "String 1 part 1 String 2 part 1\n"
            "String 1 part 2 String 2 part 2\n"
            "String 1 part 3 String 2 part 3";

        REQUIRE(multiline_concatenate(str1, str2) == res);
    }
}

TEST_CASE("strip_cr", TAGS) {
    // Stripping all CRs effectively removes all CRs
    std::string test = "aze\r\na\r\r\r\nd\r\n\r\ntest\r\n";
    std::string expected = "aze\na\nd\n\ntest\n";
    REQUIRE_NOTHROW(strip_cr(test));
    REQUIRE(test == expected);
}

} // namespace tools::string
