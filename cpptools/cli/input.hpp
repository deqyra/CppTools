#ifndef CPPTOOLS__CLI__INPUT
#define CPPTOOLS__CLI__INPUT

#include <iostream>
#include <string>

#include <cpptools/utility/string.hpp>

#include "streams.hpp"

namespace tools::cli::input
{
    // Default streams to be used
    inline cli::streams default_streams = cli::streams{};

    template<typename T>
    T ask_for_input(std::string_view title, cli::streams& streams)
    {
        // Prompt the user for a value until a valid one is entered.
        while (true)
        {
            try
            {
                streams.out << title;
                return wait_for_input<T>(streams);
            }
            catch(const std::invalid_argument&)
            {
                streams.out << "Invalid input, please enter a " << type_name<T>() << "." << std::endl;
            }            
        }
    }

    template<typename T>
    T ask_for_min_input(std::string_view title, T min, cli::streams& streams)
    {
        // Ask for input until the entered value is valid and at least min.
        while (true)
        {
            T input = ask_for_input<T>(title, streams);

            if (input >= min)
            {
                return input;
            }

            streams.out << "Invalid input, please enter a value that is greater than " << min << "." << std::endl;
        }
    }

    template<typename T>
    T ask_for_max_input(std::string_view title, T max, cli::streams& streams)
    {
        // Ask for input until the entered value is valid and at most max.
        while (true)
        {
            T input = ask_for_input<T>(title, streams);

            if (input <= max)
            {
                return input;
            }

            streams.out << "Invalid input, please enter a value that is less than " << max << "." << std::endl;
        }
    }

    template<typename T>
    T ask_for_bounded_input(std::string_view title, T min, T max, cli::streams& streams)
    {
        // Ask for input until the entered value is between min and max.
        while (true)
        {
            T input = ask_for_input<T>(title, streams);

            if (input >= min && input <= max)
            {
                return input;
            }

            streams.out << "Invalid input, please enter a value between " << min << " and " << max << "." << std::endl;
        }
    }

    template<typename T>
    T wait_for_input(cli::streams& streams)
    {
        std::string input;
        // Get input.
        std::getline(streams.in, input);
        // Take care of terminal-induced bullcrap.
        string::pop_cr(input);
        // Type-aware parsing of the input.
        return parse_string<T>(input);
    }

    template<typename T>
    T parse_string(std::string_view input) = delete;

    template<typename T>
    std::string_view type_name() = delete;

    //
    // Specializations of parse_string.
    //

    template<>
    std::string parse_string(std::string_view input);

    template<>
    int parse_string(std::string_view input);

    template<>
    bool parse_string(std::string_view input);

    //
    // Specializations of type_name.
    //

    template <>
    std::string_view type_name<std::string>();

    template <>
    std::string_view type_name<int>();

    template <>
    std::string_view type_name<bool>();

} // namespace tools::cli::input

#endif//CPPTOOLS__CLI__INPUT