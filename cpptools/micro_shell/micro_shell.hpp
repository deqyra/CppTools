#ifndef CPPTOOLS__MICRO_SHELL__MICRO_SHELL_HPP
#define CPPTOOLS__MICRO_SHELL__MICRO_SHELL_HPP

#include <string>
#include <deque>
#include <memory>
#include <vector>
#include <algorithm>

#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/utility/string.hpp>
#include <cpptools/exception/range_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>

#include "micro_shell_command.hpp"

namespace tools
{

template<typename state_t>
class micro_shell
{
public:
    enum class status_code
    {
        exit = -1,
        command_success = 0,
        command_failure = 1,
        command_bad_arguments = 2,
        command_not_found = 3,
    };

    using command = micro_shell_command<state_t>;
    using command_ptr = std::unique_ptr<command>;
    using keywords = typename command::keywords;
    using chain_iter = typename std::deque<command_ptr>::iterator;

private:
    // Command chain.
    std::deque<command_ptr> _chain;
    // Custom exit command (can be nullptr).
    command_ptr _exit_command;

public: 
    micro_shell() :
        _chain(),
        _exit_command(nullptr)
    {

    }

    //
    // Command chain manipulation
    //

    void add_command(command_ptr command)
    {
        // Do not accept several commands with the same name, nor any command named 'exit' or 'help'.
        if (has_command(command->name())
        ||  command->name() == keywords::exit
        ||  command->name() == keywords::help)
        {
            CPPTOOLS_THROW(
                exception::parameter::invalid_value_error,
                "command", command
            );
        }

        _chain.push_back(command);
    }

    void remove_command(size_t index)
    {
        // Try to erase the command at provided index.
        if (_chain.begin() + index >= _chain.end())
        {
            CPPTOOLS_THROW(
                exception::range::index_out_of_bounds_error,
                {0, _chain.size() - 1}, index
            );
        }

        _chain.erase(_chain.begin() + index);
    }

    void remove_command(const std::string& name)
    {
        // Find the index of the command with the provided name.
        size_t index = index_of(name);

        // Throw if not found.
        if (index == -1)
        {
            CPPTOOLS_THROW(
                exception::parameter::invalid_value_error,
                "name", name
            );
        }

        // Remove command.
        remove_command(index);
    }

    void set_exit_command(command_ptr command)
    {
        _exit_command = command;
    }

    void clear_commands()
    {
        _chain.clear();
    }

    //
    // Getters
    //

    command_ptr get_command(size_t index)
    {
        // Try to fetch the command at provided index.
        if (index >= _chain.size())
        {
            CPPTOOLS_THROW(
                exception::range::index_out_of_bounds_error,
                {0, _chain.size() - 1}, index
            );
        }

        return _chain.at(index);
    }

    command_ptr get_command(const std::string& name)
    {
        // Try to fetch the command at provided index.
        size_t index = index_of(name);
        if (index == -1)
        {
            CPPTOOLS_THROW(
                exception::parameter::invalid_value_error,
                "name", name
            );
        }

        return get_command(index);
    }

    command_ptr get_exit_command()
    {
        return _exit_command;
    }

    //
    // Command chain inspection
    //

    size_t index_of(const std::string& name)
    {
        // Find a command with given name.
        micro_shell<state_t>::chain_iter it = std::find_if(
            _chain.begin(), _chain.end(),
            [&name](command_ptr c) -> bool {
                return c->name() == name;
            });

        // Calculate and return index, or -1 if not found.
        return it != _chain.end() ? (it - _chain.begin()) : -1;
    }

    bool has_command(const std::string& name)
    {
        // Command exists if a valid index is found.
        return index_of(name) != (size_t)-1;
    }

    // Run the shell.
    int run(state_t& state, cli_streams& streams = cli_input::default_streams)
    {
        // Run the shell: prompt the user repeatedly and interpret the commands that were entered.
        int shell_code;
        do
        {
            std::string command = cli_input::ask_for_input<std::string>("$ ", streams);
            shell_code = process_input(command, state, streams);
        } while (shell_code != status_code::exit);      // Exit when the proper signal is returned.

        return status_code::exit;
    }

private:
    // Process user input.
    int process_input(const std::string& input, state_t& state, cli_streams& streams)
    {
        // Tokenise the string on spaces to extract the command and its arguments.
        std::vector<std::string> tokens = string::tokenize(input, ' ', true);
        if (!tokens.size()) return status_code::command_not_found;

        // If help was requested, respond accordingly.
        if (tokens[0] == keywords::help)
        {
            // If more than one token was extracted, the second one is
            // probably the command the user want to get help about.
            if (tokens.size() > 1)
            {
                streams.out << command_help_string(tokens[1]) << std::endl;;
            }
            // Otherwise, display an informative list of commands.
            else
            {
                streams.out << global_help_string() << std::endl;
            }

            return status_code::command_success;
        }

        // If exit was entered, handle the exit procedure.
        if (tokens[0] == keywords::exit)
        {
            return handle_exit(input, state, streams);
        }

        // Otherwise, search for a command sharing the name of the first token...
        size_t index = index_of(tokens[0]);
        if (index == - 1)
        {
            streams.out << tokens[0] << ": command not found.\n";
            return status_code::command_not_found;
        }

        // ...and run that command.
        try
        {
            return _chain[index]->process_input(input, state, streams);
        }
        catch(const std::exception& e)
        {
            // Informative error logging.
            streams.err << "Exception thrown by command \"" + _chain[index]->name() + "\":\n";
            streams.err << e.what() << '\n';
            streams.out << "Warning shell state may be corrupted.\n";
            streams.out << "Resuming normally..." << std::endl;
            return status_code::command_failure;
        }    
    }

    // Generate a docstring descriptive of all contained commands.
    std::string global_help_string()
    {
        // List available commands.
        std::string s = "Available commands:\n";
        for (auto it = _chain.begin(); it != _chain.end(); it++)
        {
            s += "  - " + (*it)->name() + ": " + (*it)->description() + "\n";
        }
        s += "Type 'help <command>' to get help about one command.\n";

        return s;
    }

    // Generate the docstring for a given command.
    std::string command_help_string(const std::string& command_name)
    {
        // Find command.
        size_t index = index_of(command_name);

        std::string s;
        // Handle command not found.
        if (index == -1)
        {
            s = "Command '" + command_name + "' not found, help cannot be displayed.\n";
            return s;
        }

        // Get the docstring of the command which was requested.
        s = "'" + command_name + "' help:\n";
        s += _chain[index]->help();

        return s;
    }

    // Handle exit procedure.
    int handle_exit(const std::string& command, state_t& state, cli_streams& streams)
    {
        // If not exit command was provided, just exit immediately.
        if (!_exit_command)
        {
            return status_code::exit;
        }
        // Otherwise run the provided exit command.
        return _exit_command->process_input(command, state, streams);
    }
};

} // namespace tools

#endif//CPPTOOLS__MICRO_SHELL__MICRO_SHELL_HPP