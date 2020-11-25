#ifndef TOOLS__CLI__COMMAND_SEQUENCE_HPP
#define TOOLS__CLI__COMMAND_SEQUENCE_HPP

#include <vector>
#include <string>
#include <memory>

#include "cli_command.hpp"
#include "cli_input.hpp"

// Extended command allowing to run several commands in a row.
template <typename CustomState>
class CommandSequence : public CLICommand<CustomState>
{
    using CommandPtr = std::shared_ptr<CLICommand<CustomState>>;

    private:    // Attributes
        // Ordered commands to run.
        std::vector<CommandPtr> _commandList;
        // Display name of the command sequence.
        std::string _tooltip;

    public:     // Public methods
        CommandSequence(std::vector<CommandPtr> commandList, std::string tooltip = "");
        virtual ~CommandSequence();
        
        virtual std::string getTooltip();
        // Run the command sequence.
        virtual int run(CustomState& state, CLIStreams& streams = CLIInput::defaultStreams);
};

template <typename CustomState>
CommandSequence<CustomState>::CommandSequence(std::vector<CommandSequence<CustomState>::CommandPtr> commandList, std::string tooltip) :
    CLICommand<CustomState>(),
    _commandList(commandList),
    _tooltip(tooltip)
{

}

template <typename CustomState>
CommandSequence<CustomState>::~CommandSequence()
{
    
}

template <typename CustomState>
std::string CommandSequence<CustomState>::getTooltip()
{
    if (!_tooltip.empty())
    {
        return _tooltip;
    }
    return _commandList[0]->getTooltip();
}

template <typename CustomState>
int CommandSequence<CustomState>::run(CustomState& state, CLIStreams& streams)
{
    // Run all commands in order.
    for (auto it = _commandList.begin(); it != _commandList.end(); it++)
    {
        int status;
        try
        {
            status = (*it)->run(state, streams);
        }
        catch (const std::exception& e)
        {
            // Informative error logging.
            streams.err() << "Exception thrown:\n";
            streams.err() << e.what() << '\n';
            streams.out() << "Command sequence \"" + _tooltip + "\" aborted." << std::endl;
            return CLI_COMMAND_FAILURE;
        }

        // If the command failed (but didn't throw)...
        if (status != CLI_COMMAND_SUCCESS)
        {
            // Log error and return prematurely.
            streams.out() << "Command \"" + (*it)->getTooltip() + "\" returned with value " + std::to_string(status) + "\n";
            streams.out() << "Command sequence \"" + _tooltip + "\" aborted." << std::endl;
            return status;
        }
    }

    return CLI_COMMAND_SUCCESS;
}

#endif//TOOLS__CLI__COMMAND_SEQUENCE_HPP