#ifndef CPPTOOLS__CLI__CLI_COMMAND_HPP
#define CPPTOOLS__CLI__CLI_COMMAND_HPP

#include <string>
#include <memory>

#include "cli_streams.hpp"
#include "cli_input.hpp"

namespace cpptools
{

template<typename CustomState>
class CLICommand
{
    public:
        virtual ~CLICommand();
        // Display name of the command.
        virtual std::string getTooltip() = 0;
        // Run the command.
        virtual int run(CustomState& state, CLIStreams& streams = CLIInput::defaultStreams) = 0;
};

template<typename CustomState>
CLICommand<CustomState>::~CLICommand()
{
    
}

// Command exit codes
inline static const int CLI_COMMAND_SUCCESS = 0;
inline static const int CLI_COMMAND_FAILURE = -1;

}//namespace cpptools

#endif//CPPTOOLS__CLI__CLI_COMMAND_HPP