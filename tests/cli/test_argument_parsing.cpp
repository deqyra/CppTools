#include <cpptools/cli/argument_parsing.hpp>

#include <catch2/catch_all.hpp>

#include <memory>
#include <span>
#include <string_view>

inline constexpr char TAGS[] = "[cli][arg_parse]";

namespace {

std::unique_ptr<const char*[]> make_argv(const std::span<std::string_view>& strings) {
    auto result = std::make_unique_for_overwrite<const char*[]>(strings.size());
    for (auto i = 0; i < strings.size(); ++i) {
        result[i] = strings[i].data();
    }

    return result;
}

}

namespace tools::cli::test {

TEST_CASE("argument parsing nominal case", TAGS) {
    auto arguments = std::vector<std::string_view>{
        "executable_name",
        "-a", "value",
        "--long", "value2",
        "-f",
        "-c", "a_value", "b_value",
        "--remaining", "lots", "of", "arguments", "very", "much"
    };
    auto argv = make_argv(arguments);

    value_map parsed;
    
    argument_name flag_f         = { .long_name = "flag",         .short_name = 'f'  };
    argument_name argument_a     = { .long_name = "argument",     .short_name = 'a'  };
    argument_name not_supplied_n = { .long_name = "not_supplied", .short_name = 'n'  };
    argument_name c              = { .long_name = "",             .short_name = 'c'  };
    argument_name long_arg       = { .long_name = "long",         .short_name = '\0' };
    argument_name remaining_r    = { .long_name = "remaining",    .short_name = 'r'  };
    argument_name null_arg       = { .long_name = "",             .short_name = '\0' };

    SECTION("with argument spec") {
        using argument = basic_argument<char>;
        auto arg_specs = std::vector<argument>{
            { .name = flag_f,         .necessity = necessity::optional, .value_count = 0  },
            { .name = argument_a,     .necessity = necessity::required, .value_count = 1  },
            { .name = not_supplied_n, .necessity = necessity::optional, .value_count = 1  },
            { .name = c,              .necessity = necessity::optional, .value_count = 2  },
            { .name = long_arg,       .necessity = necessity::required, .value_count = 1  },
            { .name = remaining_r,    .necessity = necessity::required, .value_count = -1 }
        };

        parsed = parse_arguments(arguments.size(), argv.get(), arg_specs);

        REQUIRE(parsed.has('f'));
        REQUIRE(parsed['f'].empty());
        REQUIRE(parsed.has("flag"));
        REQUIRE(parsed["flag"].empty());
        REQUIRE(parsed.has(flag_f));
        REQUIRE(parsed[flag_f].empty());

        REQUIRE(parsed.has('a'));
        REQUIRE(parsed['a'] == "value");
        REQUIRE(parsed.has("argument"));
        REQUIRE(parsed["argument"] == "value");
        REQUIRE(parsed.has(argument_a));
        REQUIRE(parsed[argument_a] == "value");

        REQUIRE_FALSE(parsed.has('n'));
        REQUIRE_FALSE(parsed.has("not_supplied"));
        REQUIRE_FALSE(parsed.has(not_supplied_n));
        REQUIRE_THROWS_AS(parsed['n'],            exception::lookup::no_such_element_error);
        REQUIRE_THROWS_AS(parsed["not_supplied"], exception::lookup::no_such_element_error);
        REQUIRE_THROWS_AS(parsed[not_supplied_n], exception::lookup::no_such_element_error);

        REQUIRE(parsed.has('c'));
        REQUIRE(parsed['c'] == std::vector<std::string_view>{"a_value", "b_value"});
        REQUIRE(parsed.has(c));
        REQUIRE(parsed[c] == std::vector<std::string_view>{"a_value", "b_value"});

        REQUIRE(parsed.has("long"));
        REQUIRE(parsed["long"] == "value2");
        REQUIRE(parsed.has(long_arg));
        REQUIRE(parsed[long_arg] == "value2");

        REQUIRE(parsed.has('r'));
        REQUIRE(parsed['r'] == std::vector<std::string_view>{"lots", "of", "arguments", "very", "much"});
        REQUIRE(parsed.has("remaining"));
        REQUIRE(parsed["remaining"] == std::vector<std::string_view>{"lots", "of", "arguments", "very", "much"});
        REQUIRE(parsed.has(remaining_r));
        REQUIRE(parsed[remaining_r] == std::vector<std::string_view>{"lots", "of", "arguments", "very", "much"});

        REQUIRE_THROWS_AS(parsed.has('\0'),     exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed.has(""),       exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed.has(null_arg), exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed['\0'],         exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed[""],           exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed[null_arg],     exception::parameter::null_parameter_error);
    }

    SECTION("without argument spec") {
        parsed = parse_arguments(arguments.size(), argv.get());

        REQUIRE(parsed.has('f'));
        REQUIRE(parsed['f'].empty());
        REQUIRE(parsed.has(flag_f));
        REQUIRE(parsed[flag_f].empty());

        REQUIRE(parsed.has('a'));
        REQUIRE(parsed['a'] == "value");
        REQUIRE(parsed.has(argument_a));
        REQUIRE(parsed[argument_a] == "value");

        REQUIRE_FALSE(parsed.has('n'));
        REQUIRE_FALSE(parsed.has("not_supplied"));
        REQUIRE_FALSE(parsed.has(not_supplied_n));
        REQUIRE_THROWS_AS(parsed['n'],            exception::lookup::no_such_element_error);
        REQUIRE_THROWS_AS(parsed["not_supplied"], exception::lookup::no_such_element_error);
        REQUIRE_THROWS_AS(parsed[not_supplied_n], exception::lookup::no_such_element_error);

        REQUIRE(parsed.has('c'));
        REQUIRE(parsed['c'] == std::vector<std::string_view>{"a_value", "b_value"});
        REQUIRE(parsed.has(c));
        REQUIRE(parsed[c] == std::vector<std::string_view>{"a_value", "b_value"});

        REQUIRE(parsed.has("long"));
        REQUIRE(parsed["long"] == "value2");
        REQUIRE(parsed.has(long_arg));
        REQUIRE(parsed[long_arg] == "value2");

        REQUIRE(parsed.has("remaining"));
        REQUIRE(parsed["remaining"] == std::vector<std::string_view>{"lots", "of", "arguments", "very", "much"});
        REQUIRE(parsed.has(remaining_r));
        REQUIRE(parsed[remaining_r] == std::vector<std::string_view>{"lots", "of", "arguments", "very", "much"});

        REQUIRE_THROWS_AS(parsed.has('\0'),     exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed.has(""),       exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed.has(null_arg), exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed['\0'],         exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed[""],           exception::parameter::null_parameter_error);
        REQUIRE_THROWS_AS(parsed[null_arg],     exception::parameter::null_parameter_error);
    }
}

TEST_CASE("argument parsing rejects incorrect argument specification", TAGS) {
    auto arguments = std::vector<std::string_view>{
        "executable_name"
    };
    auto argv = make_argv(arguments);
    using argument = basic_argument<char>;

    SECTION("unnamed parameter") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "", .short_name = 'v' },
                .necessity = necessity::optional,
                .value_count = 0
            },
            {
                .name = { .long_name = "", .short_name = '\0' },
                .necessity = necessity::optional,
                .value_count = 0
            },
            {
                .name = { .long_name = "valid", .short_name = '\0' },
                .necessity = necessity::optional,
                .value_count = 0
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::param_with_no_name_error);
    }

    SECTION("multiple parameters with same long name") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "valid", .short_name = 'v' },
                .necessity = necessity::optional,
                .value_count = 0
            },
            {
                .name = { .long_name = "valid", .short_name = '\0' },
                .necessity = necessity::optional,
                .value_count = 0
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::multiple_params_with_same_name_error);
    }

    SECTION("multiple parameters with same short name") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "valid", .short_name = 'v' },
                .necessity = necessity::optional,
                .value_count = 0
            },
            {
                .name = { .long_name = "", .short_name = 'v' },
                .necessity = necessity::optional,
                .value_count = 0
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::multiple_params_with_same_name_error);
    }

    SECTION("multiple parameter which consume remaining arguments") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "valid", .short_name = 'v' },
                .necessity = necessity::optional,
                .value_count = -1
            },
            {
                .name = { .long_name = "test", .short_name = 't' },
                .necessity = necessity::optional,
                .value_count = -1
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::multiple_consume_remaining_args_error);
    }
}


TEST_CASE("arg parsing returns correct errors", TAGS) {
    auto arguments = std::vector<std::string_view>{
        "executable_name",
        "-a", "value"
    };
    auto argv = make_argv(arguments);
    using argument = basic_argument<char>;

    SECTION("not enough arguments supplied in command line") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "argument", .short_name = 'a' },
                .necessity = necessity::required,
                .value_count = 2
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::not_enough_arguments_supplied_error);
    }

    SECTION("required argument not supplied in command line") {
        auto arg_specs = std::vector<argument>{
            {
                .name = { .long_name = "required", .short_name = 'r' },
                .necessity = necessity::required,
                .value_count = 1
            },
            {
                .name = { .long_name = "argument", .short_name = 'a' },
                .necessity = necessity::required,
                .value_count = 1
            }
        };

        REQUIRE_THROWS_AS(parse_arguments(arguments.size(), argv.get(), arg_specs), exception::arg_parse::required_arg_missing_error);
    }
}

}