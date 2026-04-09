#ifndef CPPTOOLS_CLI_SHELL_HPP
#define CPPTOOLS_CLI_SHELL_HPP

#include <concepts>
#include <map>
#include <string>

#include <cpptools/api.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/utility/string.hpp>
#include <cpptools/utility/heterogenous_lookup.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <string_view>

namespace tools::cli {

enum class shell_command_code {
    exit = -1,
    success = 0,
    failure = 1,
    bad_arguments = 2,
    not_found = 3,
};

template<typename E>
concept shell_environment = requires(const E& cenv, E& env) {
    { cenv.shell_prompt_head() } -> std::same_as<std::string_view>;
    { env.shell_streams() }      -> std::same_as<streams&>;
    { env.get_last_exit_code() } -> std::same_as<shell_command_code>;
    { env.set_last_exit_code(shell_command_code{}) };
};

class default_shell_environment {
    shell_command_code _last_exit_code;

public:
    virtual ~default_shell_environment() = default;

    virtual std::string_view shell_prompt_head() const {
        return "$ ";
    }

    virtual streams& shell_streams() {
        return default_streams;
    }

    virtual void set_last_exit_code(shell_command_code code) {
        _last_exit_code = code;
    }

    virtual shell_command_code get_last_exit_code() {
        return _last_exit_code;
    }
};
static_assert(shell_environment<default_shell_environment>);

template<shell_environment env_t>
class shell {
public:
    class command {
    public: 
        using code = shell_command_code;

        command() = default;
        command(const command&) = default;
        command(command&&) = default;
        virtual ~command() = default;

        virtual code run(std::string_view command, env_t& env) = 0;
    };

    using code = typename command::code;

private:
    using command_list = std::map<std::string, command*, any_transparent_less>;
    // Registered commands
    command_list _commands;

public:
    shell() = default;

    void set_command(std::string name, command& command) {
        _commands.insert({std::move(name), &command});
    }

    void remove_command(std::string_view name) {
        _commands.erase(name);
    }

    void set_default_command(command& command) {
        _commands.insert({"", &command});
    }

    void remove_default_command() {
        remove("");
    }

    void add_default_exit_command(std::string exit_command_name = "exit");

    // Run the shell.
    code run(env_t& env) {
        code shell_code = code::success;
        while (shell_code != code::exit) {
            auto command_line = prompt<std::string>(env.shell_prompt_head(), env.shell_streams());
            shell_code = _process_command_line(command_line, env);
            env.set_last_exit_code(shell_code);
        }

        return code::exit;
    }

private:
    code _process_command_line(std::string_view input, env_t& env) {
        static constexpr std::string_view Whitespace = " \t\r\n\f\v";
        input = trim(input);
        if (input.empty()) {
            return code::not_found;
        }

        auto space_pos = input.find_first_of(Whitespace);
        auto command = input.substr(0, space_pos);
        auto rest = std::string_view{};
        if (space_pos != std::string_view::npos) {
            rest = input.substr(space_pos + 1);
        }

        // run default command if not matching registered command
        auto it = _commands.find(command);
        if (it == _commands.end()) {
            auto it_default = _commands.find(""); // default command
            if (it_default != _commands.end()) {
                return _run_command(it_default, input, env);
            }

            auto& streams = env.shell_streams();
            streams.out << command << ": command not found.\n";
            return code::not_found;
        }

        // run found command otherwise
        return _run_command(it, input, env);
    }

    code _run_command(command_list::iterator it, std::string_view input, env_t& env) {
        auto& [name, command] = *it;
        try {
            return command->run(input, env);
        } catch (const std::exception& e) {
            env.shell_streams().err << std::format("Exception thrown by command \"{}\": {}\n", name, e.what());
            return code::failure;
        } catch (...) {
            env.shell_streams().err << std::format("Exception thrown by command \"{}\"\n", name);
            return code::failure;
        }
    }
};

template<typename env_t>
class default_exit_command : public shell<env_t>::command {
    using base = shell<env_t>::command;
    using code = base::code;

public:
    default_exit_command() = default;

    virtual code run(std::string_view command, env_t& env) override {
        return code::exit;
    };
};

template<shell_environment env_t>
void shell<env_t>::add_default_exit_command(std::string exit_command_name) {
    static auto exit = default_exit_command<env_t>();
    set_command(std::move(exit_command_name), exit);
}

} // namespace tools::cli

#endif//CPPTOOLS_CLI_SHELL_HPP