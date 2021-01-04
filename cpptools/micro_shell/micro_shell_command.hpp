#ifndef CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP
#define CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP

#include <string>

#include "../cli/cli_streams.hpp"
#include "../cli/cli_input.hpp"

#define HELP_KEYWORD "help"
#define EXIT_KEYWORD "exit"

namespace CppTools
{

template<typename CustomState>
class MicroShellCommand
{
    private:    // Attributes
    public:     // Public methods
        MicroShellCommand();
        virtual ~MicroShellCommand();

        virtual std::string name() = 0;
        virtual std::string description() = 0;
        virtual std::string help() = 0;
        virtual int processInput(const std::string& command, CustomState& state, CLIStreams& streams = CLIInput::defaultStreams) = 0;
};

template<typename CustomState>
MicroShellCommand<CustomState>::MicroShellCommand()
{

}

template<typename CustomState>
MicroShellCommand<CustomState>::~MicroShellCommand()
{

}

}//namespace CppTools

#endif//CPPTOOLS__MICRO_SHELL__MICRO_SHELL_COMMAND_HPP