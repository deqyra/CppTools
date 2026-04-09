#ifndef CPPTOOLS_CLI_MENU_HPP
#define CPPTOOLS_CLI_MENU_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <cpptools/utility/string.hpp>
#include <cpptools/exception/parameter_exception.hpp>

#include "command.hpp"
#include "input.hpp"

namespace tools::cli {

template<cli_environment env_t>
class menu {
public:
    using command_ptr = std::unique_ptr<command<env_t>>;

private:
    // Commands to be available in the menu.
    std::vector<command_ptr> _commands;
    // Display name of the menu.
    std::string _tooltip;
    // Display name of the exit option.
    std::string _exit_command_label;
    // Custom exit command (can be nullptr).
    command_ptr _exit_command;

public:
    template<std::size_t N>
    menu(
        command_ptr(&&commands)[N],
        std::string tooltip,
        std::string exit_command_label,
        command_ptr exit_command = nullptr
    ) :
        _commands(),
        _tooltip(std::move(tooltip)),
        _exit_command_label(std::move(exit_command_label)),
        _exit_command(std::move(exit_command))
    {
        std::size_t i = 0;
        _commands.reserve(N);
        for (auto& c : commands) {
            if (c == nullptr) {
                CPPTOOLS_THROW(exception::parameter::null_parameter_error, "commands[" + std::to_string(i) + "]");
            }
            ++i;
            _commands.push_back(std::move(c));
        }
    }

    menu(menu&& other) = default;

    std::string_view tooltip() const {
        return _tooltip;
    }

    // Print the menu and handle input.
    void show(env_t& env) {
        auto& streams = env.cli_streams();

        bool exit_requested = false;
        while (not exit_requested) {
            // Print menu header and options.
            streams.out << std::format("\n{}:\n{}", _tooltip, _all_option_strings());
            // Prompt the user.
            int input = cli::prompt_bounded<std::size_t>("Please make a choice: ", 0, _commands.size(), streams);

            // Handle exit if required.
            if (input == 0) {
                exit_requested = true;
                if (_exit_command) {
                    _exit_command->run(env);
                }
                break;
            }

            // Run the corresponding command otherwise.
            streams.out << '\n';
            streams.out << _commands[input - 1]->tooltip() << ":" << std::endl;

            try {
                _commands[input - 1]->run(env);
            } catch (const std::exception& e) {
                // Informative error logging.
                streams.err << std::format("Exception thrown by command \"{}\":\n{}", _commands[input - 1]->tooltip(), e.what()) << std::endl;
            }
        }
    }

private:
    // Generate display string for a given tooltip.
    std::string _option_string(int n, std::string_view tooltip) {
        return std::format("{}. {}\n", n, tooltip);
    }

    // Generate display strings for all options.
    std::string_view _all_option_strings() {
        static std::string s = [this](){
            std::string result;
            for (int i = 0; i < _commands.size(); i++) {
                result += _option_string(i + 1, _commands[i]->tooltip());
            }
            result += _option_string(0, _exit_command_label);

            return result;
        }();

        return s;
    }
};

} // namespace tools

#endif//CPPTOOLS_CLI_MENU_HPP