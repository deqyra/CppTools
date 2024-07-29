#ifndef CPPTOOLS_CLI_SHELL_HPP
#define CPPTOOLS_CLI_SHELL_HPP

#include <algorithm>
#include <map>
#include <set>
#include <string>

#include <cpptools/api.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/utility/string.hpp>
#include <cpptools/exception/parameter_exception.hpp>

namespace tools::cli {

enum class shell_command_code {
    exit = -1,
    success = 0,
    failure = 1,
    bad_arguments = 2,
    not_found = 3,
};

CPPTOOLS_API inline std::string to_string(shell_command_code c) {
    switch (c) {
        case shell_command_code::exit:
            return "exit";
        case shell_command_code::success:
            return "success";
        case shell_command_code::failure:
            return "failure";
        case shell_command_code::bad_arguments:
            return "bad_arguments";
        case shell_command_code::not_found:
            return "not_found";
        default:
            return "???";
    }
}

namespace shell_command_keywords {
    static constexpr std::string_view help = "help";
    static constexpr std::string_view exit = "exit";
    static constexpr std::string_view _all_keywords = "help, exit";
};

template<typename context_t>
class shell {
public:
    class command {
    public: 
        using code = shell_command_code;
        
        command() = default;
        command(const command&) = default;
        command(command&&) = default;
        virtual ~command() = default;

        virtual std::string name() const = 0;
        virtual std::string description() const = 0;
        virtual std::string help() const = 0;
        virtual code process_input(std::string_view command, context_t& state, streams& streams = default_streams) = 0;
    };

    using command_ptr = std::unique_ptr<command>;
    using code = typename command::code;

private:
    // Registered commands
    std::map<std::string, command_ptr> _commands;
    // Custom exit command (can be nullptr).
    command_ptr _exit_command;

public:
    template<size_t N>
    shell(std::array<command_ptr, N>&& commands, command_ptr exit_command = nullptr) :
        _commands(),
        _exit_command(std::move(exit_command))
    {
        std::set<std::string_view> command_names;

        for (const auto& c : commands) {
            const auto& name = c->name();
            if (_is_forbidden_command_name(name)) {
                CPPTOOLS_THROW(exception::parameter::invalid_value_error, "commands[...]", std::string{name});
            }
            if(command_names.find(name) != command_names.end()) {
                CPPTOOLS_THROW(exception::parameter::invalid_value_error, "commands[...]", std::string{name});
            }
            command_names.insert(name);
        }

        for (auto& c : commands) {
            _commands.insert_or_assign(std::string{c->name()}, std::move(c));
        }
    }

    //
    // Command chain manipulation
    //

    void add_command(command_ptr&& command) {
        // Do not accept several commands with the same name, nor any command named 'exit' or 'help'.
        auto name = command->name();

        if (!_is_valid_command_name(name)) {
            CPPTOOLS_THROW(exception::parameter::invalid_value_error, "command", std::string{name});
        }

        _commands.insert({std::string{name}, std::move(command)});
    }

    command_ptr remove_command(const std::string& name) {
        auto it = _commands.find(name);
        if (it == _commands.end()) {
            CPPTOOLS_THROW(exception::parameter::invalid_value_error, "name", std::string{name});
        }

        command_ptr removed = std::move(it->second); 
        // Remove command.
        _commands.erase(name);

        return removed;
    }

    command_ptr set_exit_command(command_ptr&& command) {
        command_ptr previous_exit_command = std::move(_exit_command);
        _exit_command = std::move(command);

        return previous_exit_command;
    }

    void clear_commands() {
        _commands.clear();
    }

    //
    // Getters
    //

    command& get_command(const std::string& name) {
        // Try to fetch the command at provided index.
        auto it = _commands.find(name);
        if (it == _commands.end()) {
            CPPTOOLS_THROW(exception::parameter::invalid_value_error, "name", std::string{name});
        }

        return *(it->second);
    }

    command& get_exit_command() {
        return *_exit_command;
    }

    //
    // Command chain inspection
    //

    bool has_command(const std::string& name) {
        return _commands.find(name) != _commands.end();
    }

    // Run the shell.
    code run(context_t& state, streams& streams = default_streams) {
        // Run the shell: prompt the user repeatedly and interpret the commands that were entered.
        code shell_code = code::success;
        while (shell_code != code::exit) {
            std::string command_line = prompt<std::string>("$ ", streams);
            shell_code = _process_input(command_line, state, streams);
        }

        return code::exit;
    }

private:
    // Process user input.
    code _process_input(const std::string& input, context_t& state, streams& streams) {
        // Tokenise the string on spaces to extract the command and its arguments.
        std::vector<std::string> tokens = tokenize(input, ' ', true);
        if (!tokens.size()) {
            return code::not_found;
        }

        // If help was requested, respond accordingly.
        if (tokens[0] == shell_command_keywords::help) {
            // If more than one token was extracted, the second one is
            // probably the command the user want to get help about.
            if (tokens.size() > 1) {
                streams.out << _command_help_string(tokens[1]) << std::endl;;
            } else { // Otherwise, display an informative list of commands.
                streams.out << _global_help_string() << std::endl;
            }

            return code::success;
        }

        // If exit was entered, handle the exit procedure.
        if (tokens[0] == shell_command_keywords::exit) {
            return _handle_exit(input, state, streams);
        }

        // Otherwise, search for a command sharing the name of the first token...
        auto it = _commands.find(tokens[0]);
        if (it == _commands.end()) {
            streams.out << tokens[0] << ": command not found.\n";
            return code::not_found;
        }

        // ...and run that command.
        try {
            return it->second->process_input(input, state, streams);
        } catch (const std::exception& e) {
            // Informative error logging.
            streams.err << "Exception thrown by command \"" + it->second->name() + "\":\n";
            streams.err << e.what() << '\n';
            streams.out << "Warning shell state may be corrupted.\n";
            streams.out << "Resuming normally..." << std::endl;
            return code::failure;
        }
    }

    // Generate a docstring descriptive of all contained commands.
    std::string _global_help_string() {
        // List available commands.
        std::string s = "Available commands:\n";
        for (auto& [name, command] : _commands) {
            s += "  - " + command->name() + ": " + command->description() + "\n";
        }
        s += "Type 'help <command>' to get help about one command.\n";

        return s;
    }

    // Generate the docstring for a given command.
    std::string _command_help_string(const std::string& name) {
        std::string s;

        auto it = _commands.find(name);
        if (it == _commands.end()) {
            CPPTOOLS_THROW(exception::parameter::invalid_value_error, "command_name", std::string{name});
        }

        // Get the docstring of the command which was requested.
        s = "'" + std::string(name) + "' help:\n";
        s += _commands[name]->help();

        return s;
    }

    // Handle exit procedure.
    code _handle_exit(const std::string& name, context_t& state, streams& streams) {
        // If not exit command was provided, just exit immediately.
        if (!_exit_command) {
            return code::exit;
        }
        // Otherwise run the provided exit command.
        return _exit_command->process_input(name, state, streams);
    }

    bool _is_valid_command_name(const std::string& name) {
        return !_is_forbidden_command_name(name) 
            && (_commands.find(name) == _commands.end());
    }

    static bool _is_forbidden_command_name(const std::string& name) {
        return (name == shell_command_keywords::exit)
            || (name == shell_command_keywords::help);
    }
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_SHELL_HPP