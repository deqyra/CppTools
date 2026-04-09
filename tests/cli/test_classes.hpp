#ifndef TESTS_CLI_TEST_CLASSES_HPP
#define TESTS_CLI_TEST_CLASSES_HPP

#include <fstream>
#include <memory>
#include <sstream>

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/command.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>
#include <cpptools/cli/shell.hpp>

namespace tools::cli
{

struct test_env : default_shell_environment, default_cli_environment {
public:
    std::ifstream in;
    std::stringstream out;
    std::stringstream err;
    streams st{ in, out, err };

    test_env(const std::filesystem::path& input_file)
        : in(input_file, std::ios::in)
        , out()
        , err()
    {

    }

    streams& shell_streams() override {
        return st;
    }

    streams& cli_streams() override {
        return st;
    }
};
static_assert(shell_environment<test_env>);
static_assert(cli_environment<test_env>);

using test_menu = menu<test_env>;
using test_command = command<test_env>;
using test_command_ptr = std::unique_ptr<test_command>;
using test_menu_command = menu_command<test_env>;
using test_command_sequence = command_sequence<test_env>;

class test_command1 : public test_command {
public:
    test_command1() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_env& env) override;
};

class test_command2 : public test_command {
public:
    test_command2() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_env& env) override;
};

class test_exit_command : public test_command {
public:
    test_exit_command() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_env& env) override;
};

test_menu make_basic_test_menu();
test_command_sequence make_basic_test_command_sequence();

using test_shell = shell<test_env>;
using test_shell_command = typename shell<test_env>::command;
using test_shell_command_ptr = std::unique_ptr<test_shell_command>;

class test_shell_cmd : public test_shell_command {
    public:
        test_shell_cmd() = default;
        virtual code run(std::string_view command, test_env& env) override;
};

class test_shell_cmd_throw : public test_shell_command {
    public:
        test_shell_cmd_throw() = default;
        virtual code run(std::string_view command, test_env& env) override;
};

class test_shell_exit_command : public test_shell_command {
    public:
        test_shell_exit_command() = default;
        virtual code run(std::string_view command, test_env& env) override;
};

class test_shell_help_command : public test_shell_command {
    public:
        test_shell_help_command() = default;
        virtual code run(std::string_view command, test_env& env) override;
};

} // namespace tools::cli

#endif//TESTS_CLI_TEST_CLASSES_HPP