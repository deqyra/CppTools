#ifndef CPPTOOLS_CLI_INPUT
#define CPPTOOLS_CLI_INPUT

#include <concepts>
#include <iostream>
#include <limits>
#include <string>

#include <cpptools/api.hpp>
#include <cpptools/utility/string.hpp>

#include "streams.hpp"

namespace tools::cli {

template<typename F, typename T>
concept parse_func = requires(const F& f) {
    { f(std::string_view{}) } -> std::convertible_to<T>;
};

template<typename F, typename T>
concept type_name_func = requires(const F& f) {
    { f() } -> std::same_as<std::string>;
};

template<typename T>
struct parse{};

template<typename T>
struct type_name{};

// Specializations of parse_as / type_name for std::string

template<>
struct parse<std::string> {
    CPPTOOLS_API std::string operator()(std::string_view input) const {
        return std::string(input);
    }
};
static_assert(parse_func<parse<std::string>, std::string>);

template<>
struct type_name<std::string> {
    CPPTOOLS_API std::string operator()() const {
        return "a string";
    }
};
static_assert(type_name_func<type_name<std::string>, std::string>);

// Specializations of parse_as / type_name for integers

template<std::integral T>
struct parse<T> {
    CPPTOOLS_API T operator()(std::string_view input) const {
        if (!is_integer(input)) {
            throw std::invalid_argument("parse_string<integer>: String to parse is not exclusively made of digits and a minus sign, or it is at a wrong position.");
        }

        long long result = std::stoll(std::string(input));
        if (result > std::numeric_limits<T>::max() ||
            result < std::numeric_limits<T>::min())
        {
            throw std::invalid_argument("parse_string<integer>: parsed result does not fit in return type.");
        }

        return static_cast<T>(result);
    }
};
static_assert(parse_func<parse<char>, char>);
static_assert(parse_func<parse<signed char>, signed char>);
static_assert(parse_func<parse<unsigned char>, unsigned char>);
static_assert(parse_func<parse<short>, short>);
static_assert(parse_func<parse<unsigned short>, unsigned short>);
static_assert(parse_func<parse<int>, int>);
static_assert(parse_func<parse<unsigned int>, unsigned int>);
static_assert(parse_func<parse<long long>, long long>);
static_assert(parse_func<parse<unsigned long long>, unsigned long long>);

template<std::integral T> requires (not std::same_as<T, bool>)
struct type_name<T> {
    CPPTOOLS_API std::string operator()() const {
        return std::format(
            "an integer ({} to {})",
            std::numeric_limits<T>::min(),
            std::numeric_limits<T>::max()
        );
    }
};

static_assert(type_name_func<type_name<char>, char>);
static_assert(type_name_func<type_name<signed char>, signed char>);
static_assert(type_name_func<type_name<unsigned char>, unsigned char>);
static_assert(type_name_func<type_name<short>, short>);
static_assert(type_name_func<type_name<unsigned short>, unsigned short>);
static_assert(type_name_func<type_name<int>, int>);
static_assert(type_name_func<type_name<unsigned int>, unsigned int>);
static_assert(type_name_func<type_name<long long>, long long>);
static_assert(type_name_func<type_name<unsigned long long>, unsigned long long>);

// Specializations of parse_as / type_name for bool

template<>
struct parse<bool> {
    CPPTOOLS_API bool operator()(std::string_view input) const {
        if (input == "y") {
            return true;
        }

        if (input == "n") {
            return false;
        }

        throw std::invalid_argument("parse_string<parse_yn_boolean>: Invalid string value for expected bool input.");
    }
};
static_assert(parse_func<parse<bool>, bool>);

template<>
struct type_name<bool> {
    CPPTOOLS_API std::string operator()() const {
        return "(y/n)";
    }
};
static_assert(type_name_func<type_name<bool>, bool>);

template<typename T>
T read_input(cli::streams& streams) {
    std::string input;

    std::getline(streams.in, input);
    pop_cr(input);

    return parse<T>{}(input);
}

template<typename T>
T prompt(std::string_view title, streams& streams) {
    while (true) {
        try {
            streams.out << title;
            return read_input<T>(streams);
        } catch(const std::invalid_argument&) {
            streams.out << "Please enter " << type_name<T>{}() << "." << std::endl;
        }
    }
}

template<typename T>
T prompt_min(std::string_view title, T min, streams& streams) {
    while (true) {
        T input = prompt<T>(title, streams);

        if (input >= min) {
            return input;
        }

        streams.out << "Please enter a value greater than " << min << "." << std::endl;
    }
}

template<typename T>
T prompt_max(std::string_view title, T max, streams& streams) {
    while (true) {
        T input = prompt<T>(title, streams);

        if (input <= max) {
            return input;
        }

        streams.out << "Please enter a value less than " << max << "." << std::endl;
    }
}

template<typename T>
T prompt_bounded(std::string_view title, T min, T max, cli::streams& streams) {
    while (true) {
        T input = prompt<T>(title, streams);

        if (input >= min && input <= max) {
            return input;
        }

        streams.out << "Please enter a value between " << min << " and " << max << "." << std::endl;
    }
}

} // namespace tools

#endif//CPPTOOLS_CLI_INPUT