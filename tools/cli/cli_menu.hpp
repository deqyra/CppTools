#ifndef TOOLS__CLI__CLI_MENU_HPP
#define TOOLS__CLI__CLI_MENU_HPP

#include <string>
#include <vector>
#include <memory>

#include "cli_streams.hpp"
#include "cli_command.hpp"
#include "cli_input.hpp"
#include "../../tools/string_tools.hpp"

template<typename CustomState>
class CLIMenu
{
    using CommandPtr = std::shared_ptr<CLICommand<CustomState>>;
    
    private:    // Attributes
        // Commands to be available in the menu.
        std::vector<CommandPtr> _commands;
        // Display name of the menu.
        std::string _tooltip;
        // Display name of the exit option.
        std::string _exitName;
        // Custom exit command (can be nullptr).
        CommandPtr _exitCommand;

    public:     // Public methods
        CLIMenu(std::vector<CommandPtr> commands, std::string tooltip, std::string exitName, CommandPtr exitCommand = nullptr);
        
        std::string getTooltip();
        // Print the menu and handle input.
        void show(CustomState& state, CLIStreams& streams = CLIInput::defaultStreams);
        
    private:    // Private methods
        // Generate display string for a given tooltip.
        std::string optionString(int n, const std::string& tooltip);
        // Generate display strings for all options.
        std::string allOptionsString();
};

template<typename CustomState>
CLIMenu<CustomState>::CLIMenu(std::vector<CLIMenu<CustomState>::CommandPtr> commands, std::string tooltip, std::string exitName, CLIMenu<CustomState>::CommandPtr exitCommand) :
    _commands(commands),
    _tooltip(tooltip),
    _exitName(exitName),
    _exitCommand(exitCommand)
{

}

template<typename CustomState>
std::string CLIMenu<CustomState>::getTooltip()
{
    return _tooltip;
}

template<typename CustomState>
void CLIMenu<CustomState>::show(CustomState& state, CLIStreams& streams)
{
    // While user did not ask to exit...
    while (true)
    {
        // Print menu header and options.
        streams.out() << '\n';
        streams.out() << getTooltip() << ":" << std::endl;
        streams.out() << allOptionsString();

        // Prompt the user.
        int nOptions = _commands.size();
        int input = CLIInput::askForBoundedInput<int>("Please make a choice: ", 0, nOptions, streams);

        // Handle exit if required.
        if (input == 0)
        {
            if (_exitCommand)
            {
                _exitCommand->run(state, streams);
            }
            break;
        }

        // Run the corresponding command otherwise.
        streams.out() << '\n';
        streams.out() << _commands[input - 1]->getTooltip() << ":" << std::endl;

        try
        {
            _commands[input - 1]->run(state, streams);
        }
        catch (const std::exception& e)
        {
            // Informative error logging.
            streams.err() << "Exception thrown by command \"" + _commands[input - 1]->getTooltip() + "\":\n";
            streams.err() << e.what() << '\n';
            streams.out() << "Aborting." << std::endl;
        }
    }
}

template<typename CustomState>
std::string CLIMenu<CustomState>::optionString(int n, const std::string& tooltip)
{
    std::string s = std::to_string(n) + ". " + tooltip + "\n";
    return s;
}

template<typename CustomState>
std::string CLIMenu<CustomState>::allOptionsString()
{
    std::string s;

    for (int i = 0; i < _commands.size(); i++)
    {
        s += optionString(i + 1, _commands[i]->getTooltip());
    }
    s += optionString(0, _exitName);

    return s;
}

#endif//TOOLS__CLI__CLI_MENU_HPP