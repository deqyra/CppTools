#ifndef CPPTOOLS__CLI__CLI_MENU_HPP
#define CPPTOOLS__CLI__CLI_MENU_HPP

#include <string>
#include <vector>
#include <memory>

#include <cpptools/utility/string.hpp>

#include "cli_streams.hpp"
#include "cli_command.hpp"
#include "cli_input.hpp"

namespace tools
{

template<typename state_t>
class cli_menu
{
    using command_ptr = std::unique_ptr<cli_command<state_t>>;

private:
    // Commands to be available in the menu.
    std::vector<command_ptr> _commands;
    // Display name of the menu.
    std::string _tooltip;
    // Display name of the exit option.
    std::string _exit_name;
    // Custom exit command (can be nullptr).
    command_ptr _exit_command;

public:
    cli_menu(
        std::vector<command_ptr> commands,
        std::string tooltip,
        std::string exit_name,
        command_ptr exit_command = nullptr
    ) :
        _commands(commands),
        _tooltip(tooltip),
        _exit_name(exit_name),
        _exit_command(exit_command)
    {

    }

    std::string get_tooltip()
    {
        return _tooltip;
    }

    // Print the menu and handle input.
    void show(state_t& state, cli_streams& streams = cli_input::default_streams)
    {
        // While user did not ask to exit...
        while (true)
        {
            // Print menu header and options.
            streams.out << '\n';
            streams.out << get_tooltip() << ":" << std::endl;
            streams.out << all_options_string();

            // Prompt the user.
            int n_options = (int)_commands.size();
            int input = cli_input::ask_for_bounded_input<int>("Please make a choice: ", 0, n_options, streams);

            // Handle exit if required.
            if (input == 0)
            {
                if (_exit_command)
                {
                    _exit_command->run(state, streams);
                }
                break;
            }

            // Run the corresponding command otherwise.
            streams.out << '\n';
            streams.out << _commands[input - 1]->get_tooltip() << ":" << std::endl;

            try
            {
                _commands[input - 1]->run(state, streams);
            }
            catch (const std::exception& e)
            {
                // Informative error logging.
                streams.err << "Exception thrown by command \"" + _commands[input - 1]->get_tooltip() + "\":\n";
                streams.err << e.what() << '\n';
                streams.out << "Aborting." << std::endl;
            }
        }
    }

private:
    // Generate display string for a given tooltip.
    std::string option_string(int n, const std::string& tooltip)
    {
        std::string s = std::to_string(n) + ". " + tooltip + "\n";
        return s;
    }

    // Generate display strings for all options.
    std::string all_options_string()
    {
        std::string s;

        for (int i = 0; i < _commands.size(); i++)
        {
            s += option_string(i + 1, _commands[i]->get_tooltip());
        }
        s += option_string(0, _exit_name);

        return s;
    }
};

} // namespace tools

#endif//CPPTOOLS__CLI__CLI_MENU_HPP