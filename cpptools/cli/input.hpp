#ifndef CPPTOOLS_CLI_INPUT
#define CPPTOOLS_CLI_INPUT

#include <iostream>
#include <string>

#include <cpptools/api.hpp>
#include <cpptools/utility/string.hpp>

#include "streams.hpp"

namespace tools::cli {

namespace detail {

    template<typename T>
    T parse_as(std::string_view input) = delete;

    template<typename T>
    std::string_view type_name() = delete;

    //
    // Specializations of parse_string
    //

    template<>
    CPPTOOLS_API inline std::string parse_as(std::string_view input) {
        return std::string(input);
    }

    template<>
    CPPTOOLS_API inline int parse_as(std::string_view input) {
        if (!is_integer(input)) {
            throw std::invalid_argument("parse_string<int>: String to parse is not exclusively made of digits and a minus sign, or it is at a wrong position.");
        }
        return std::stoi(std::string(input));
    }

    template<>
    CPPTOOLS_API inline bool parse_as(std::string_view input) {
        if (input == "y" || input == "yes" || input == "true") {
            return true;
        }

        if (input == "n" || input == "no" || input == "false") {
            return false;
        }

        throw std::invalid_argument("parse_string<bool>: Invalid string value for expected bool input.");
    }

    //
    // Specializations of type_name
    //

    template <>
    CPPTOOLS_API inline std::string_view type_name<std::string>() {
        return "string";
    }

    template <>
    CPPTOOLS_API inline std::string_view type_name<int>() {
        return "integer";
    }

    template <>
    CPPTOOLS_API inline std::string_view type_name<bool>() {
        return "boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\")";
    }

} // namespace detail

template<typename T>
T read_input(cli::streams& streams) {
    std::string input;

    std::getline(streams.in, input);
    pop_cr(input);

    return detail::parse_as<T>(input);
}

template<typename T>
T prompt(std::string_view title, streams& streams) {
    while (true) {
        try {
            streams.out << title;
            return read_input<T>(streams);
        }
        catch(const std::invalid_argument&) {
            streams.out << "Please enter a " << detail::type_name<T>() << "." << std::endl;
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