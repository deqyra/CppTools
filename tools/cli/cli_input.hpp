#ifndef TOOLS__CLI__CLI_INPUT
#define TOOLS__CLI__CLI_INPUT

#include <iostream>
#include <string>

#include "cli_streams.hpp"
#include "../string_tools.hpp"

namespace CLIInput
{
    // Default streams to be used
    inline static CLIStreams defaultStreams = CLIStreams(std::cin, std::cout, std::cerr);

    // Prompts the user for input using the provided string as title. Loops until input is successfully parsed.
    template<typename T>
    static T askForInput(const std::string& title, CLIStreams& streams = defaultStreams);

    // Repeatedly asks for input until entered value is at least the provided minimum value. Should be used on orderable types when it makes sense.
    template<typename T>
    static T askForMinInput(const std::string& title, T min, CLIStreams& streams = defaultStreams);

    // Repeatedly asks for input until entered value is at most the provided maximum value. Should be used on orderable types when it makes sense.
    template<typename T>
    static T askForMaxInput(const std::string& title, T max, CLIStreams& streams = defaultStreams);

    // Repeatedly asks for input until entered value is within provided bounds. Should be used on orderable types when it makes sense.
    template<typename T>
    static T askForBoundedInput(const std::string& title, T min, T max, CLIStreams& streams = defaultStreams);

    // Parses a string to find a value of the template parameter. Needs an explicit specialized definition for each type.
    template<typename T>
    static T parseString(const std::string& input);

    // Returns a descriptive string of the template parameter. Needs an explicit specialized definition for each type.
    template<typename T>
    static std::string typeName();

    template<typename T>
    static T waitForInput(CLIStreams& streams = defaultStreams);

    template<typename T>
    T askForInput(const std::string& title, CLIStreams& streams)
    {
        // Prompt the user for a value until a valid one is entered.
        while (true)
        {
            try
            {
                streams.out() << title;
                return waitForInput<T>(streams);
            }
            catch(const std::invalid_argument& e)
            {
                streams.out() << "Invalid input, please enter a " << typeName<T>() << "." << std::endl;
            }            
        }
    }

    template<typename T>
    T askForMinInput(const std::string& title, T min, CLIStreams& streams)
    {
        // Ask for input until the entered value is valid and at least min.
        while (true)
        {
            T input = askForInput<T>(title, streams);

            if (input >= min)
            {
                return input;
            }

            streams.out() << "Invalid input, please enter a value that is at least " << min << "." << std::endl;
        }
    }

    template<typename T>
    T askForMaxInput(const std::string& title, T max, CLIStreams& streams)
    {
        // Ask for input until the entered value is valid and at most max.
        while (true)
        {
            T input = askForInput<T>(title, streams);

            if (input <= max)
            {
                return input;
            }

            streams.out() << "Invalid input, please enter a value that is at most " << max << "." << std::endl;
        }
    }

    template<typename T>
    T askForBoundedInput(const std::string& title, T min, T max, CLIStreams& streams)
    {
        // Ask for input until the entered value is between min and max.
        while (true)
        {
            T input = askForInput<T>(title, streams);

            if (input >= min && input <= max)
            {
                return input;
            }

            streams.out() << "Invalid input, please enter a value between " << min << " and " << max << "." << std::endl;
        }
    }

    template<typename T>
    T waitForInput(CLIStreams& streams)
    {
        std::string input;
        // Get input.
        std::getline(streams.in(), input);
        // Take care of terminal-induced bullcrap.
        StringTools::popCR(input);
        // Type-aware parsing of the input.
        return parseString<T>(input);
    }

    //
    // Specializations of parseString.
    //

    template<>
    std::string parseString(const std::string& input)
    {
        return input;
    }

    template<>
    int parseString(const std::string& input)
    {
        if (!StringTools::stringIsNum(input))
        {
            throw std::invalid_argument("parseString<int>: String to parse is not exclusively made of digits and a minus sign, or it is at a wrong position.");
        }
        return std::stoi(input);
    }

    template<>
    bool parseString(const std::string& input)
    {
        if (input == "y" || input == "yes" || input == "true")
        {
            return true;
        }

        if (input == "n" || input == "no" || input == "false")
        {
            return false;
        }

        throw std::invalid_argument("parseString<bool>: Invalid string value for expected bool input.");
    }

    //
    // Specializations of typeName.
    //

    template <>
    std::string typeName<std::string>()
    {
        return "string";
    }

    template <>
    std::string typeName<int>()
    {
        return "integer";
    }

    template <>
    std::string typeName<bool>()
    {
        return "boolean (\"y\", \"yes\", \"true\", \"n\", \"no\", \"false\")";
    }
} // namespace CLIInput

#endif//TOOLS__CLI__CLI_INPUT