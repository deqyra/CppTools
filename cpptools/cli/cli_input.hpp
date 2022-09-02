#ifndef CPPTOOLS__CLI__CLI_INPUT
#define CPPTOOLS__CLI__CLI_INPUT

#include <iostream>
#include <string>

#include <cpptools/utility/string.hpp>

#include "cli_streams.hpp"

namespace tools::cli_input
{
    // Default streams to be used
    inline cli_streams default_streams = cli_streams{};

    template<typename T>
    T ask_for_input(const std::string& title, cli_streams& streams)
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
    T ask_for_min_input(const std::string& title, T min, cli_streams& streams)
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
    T ask_for_max_input(const std::string& title, T max, cli_streams& streams)
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
    T ask_for_bounded_input(const std::string& title, T min, T max, cli_streams& streams)
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
    T wait_for_input(cli_streams& streams)
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
    T parse_string(const std::string& input) = delete;

    template<typename T>
    std::string type_name() = delete;

    //
    // Specializations of parse_string.
    //

    template<>
    std::string parse_string(const std::string& input);

    template<>
    int parse_string(const std::string& input);

    template<>
    bool parse_string(const std::string& input);

    //
    // Specializations of type_name.
    //

    template <>
    std::string type_name<std::string>();

    template <>
    std::string type_name<int>();

    template <>
    std::string type_name<bool>();

} // namespace tools::cli_input

#endif//CPPTOOLS__CLI__CLI_INPUT