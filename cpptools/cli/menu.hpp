#ifndef CPPTOOLS__CLI__MENU_HPP
#define CPPTOOLS__CLI__MENU_HPP

#include <string>
#include <vector>
#include <memory>

#include <cpptools/utility/string.hpp>
#include <cpptools/exception/parameter_exception.hpp>

#include "streams.hpp"
#include "command.hpp"
#include "input.hpp"

namespace tools::cli
{

template<typename context_t, std::size_t N>
class menu
{
public:
    using command_ptr = std::unique_ptr<cli::command<context_t>>;

private:
    // Commands to be available in the menu.
    std::array<command_ptr, N> _commands;
    // Display name of the menu.
    std::string _tooltip;
    // Display name of the exit option.
    std::string _exit_name;
    // Custom exit command (can be nullptr).
    command_ptr _exit_command;

public:
    menu(
        std::array<command_ptr, N>&& commands,
        std::string tooltip,
        std::string exit_name,
        command_ptr exit_command = nullptr
    ) :
        _commands(std::move(commands)),
        _tooltip(tooltip),
        _exit_name(exit_name),
        _exit_command(std::move(exit_command))
    {
        std::size_t i = 0;
        for (auto& c : _commands)
        {
            if (c == nullptr)
            {
                CPPTOOLS_THROW(exception::parameter::null_parameter_error, "commands[" + std::to_string(i) + "]");
            }
            ++i;
        }
    }

    menu(menu&& other) = default;

    std::string tooltip() const
    {
        return _tooltip;
    }

    // Print the menu and handle input.
    void show(context_t& state, cli::streams& streams = cli::input::default_streams)
    {
        // While user did not ask to exit...
        while (true)
        {
            // Print menu header and options.
            streams.out << '\n';
            streams.out << tooltip() << ":" << std::endl;
            streams.out << _all_option_strings();

            // Prompt the user.
            int n_options = (int)_commands.size();
            int input = cli::input::ask_for_bounded_input<int>("Please make a choice: ", 0, n_options, streams);

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
            streams.out << _commands[input - 1]->tooltip() << ":" << std::endl;

            try
            {
                _commands[input - 1]->run(state, streams);
            }
            catch (const std::exception& e)
            {
                // Informative error logging.
                streams.err << "Exception thrown by command \"" + _commands[input - 1]->tooltip() + "\":\n";
                streams.err << e.what() << '\n';
                streams.out << "Aborting." << std::endl;
            }
        }
    }

private:
    // Generate display string for a given tooltip.
    std::string _option_string(int n, std::string_view tooltip)
    {
        std::string s = std::to_string(n) + ". " + std::string(tooltip) + "\n";
        return s;
    }

    // Generate display strings for all options.
    std::string_view _all_option_strings()
    {
        static std::string s;
        static bool run_once = false;

        if (!run_once)
        {
            for (int i = 0; i < _commands.size(); i++)
            {
                s += _option_string(i + 1, _commands[i]->tooltip());
            }
            s += _option_string(0, _exit_name);

            run_once = true;
        }

        return s;
    }
};

} // namespace tools

#endif//CPPTOOLS__CLI__MENU_HPP