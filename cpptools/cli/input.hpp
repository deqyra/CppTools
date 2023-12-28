#ifndef CPPTOOLS__CLI__INPUT
#define CPPTOOLS__CLI__INPUT

#include <iostream>
#include <string>

#include <cpptools/utility/string.hpp>

#include "streams.hpp"

namespace tools {

namespace detail {

    template<typename T>
    T parse_as(std::string_view input) = delete;

    template<typename T>
    std::string_view type_name() = delete;

    //
    // Specializations of parse_string
    //

    template<>
    std::string parse_as(std::string_view input);

    template<>
    int parse_as(std::string_view input);

    template<>
    bool parse_as(std::string_view input);

    //
    // Specializations of type_name
    //

    template <>
    std::string_view type_name<std::string>();

    template <>
    std::string_view type_name<int>();

    template <>
    std::string_view type_name<bool>();

} // namespace detail

namespace cli::input {

    // Default streams to be used
    inline streams default_streams;

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

    template<typename T>
    T read_input(cli::streams& streams) {
        std::string input;

        std::getline(streams.in, input);
        string::pop_cr(input);

        return detail::parse_as<T>(input);
    }

} // namespace cli::input

} // namespace tools

#endif//CPPTOOLS__CLI__INPUT