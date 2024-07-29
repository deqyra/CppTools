#ifndef TESTS_CLI_TEST_CLASSES_HPP
#define TESTS_CLI_TEST_CLASSES_HPP

#include <memory>

#include <cpptools/cli/menu.hpp>
#include <cpptools/cli/command.hpp>
#include <cpptools/cli/streams.hpp>
#include <cpptools/cli/input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>
#include <cpptools/cli/shell.hpp>

namespace tools::cli
{

class test_state {};

template<std::size_t N>
using test_menu = menu<test_state, N>;
using test_command = command<test_state>;
using test_command_ptr = std::unique_ptr<test_command>;
template<std::size_t N>
using test_menu_command = menu_command<test_state, N>;
template<std::size_t N>
using test_command_sequence = command_sequence<test_state, N>;

class test_command1 : public test_command {
public:
    test_command1() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_state& state, streams& streams) override;
};

class test_command2 : public test_command {
public:
    test_command2() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_state& state, streams& streams) override;
};

class test_exit_command : public test_command {
public:
    test_exit_command() : test_command() {}
    std::string_view tooltip() const override;
    code run(test_state& state, streams& streams) override;
};

test_menu<2> make_basic_test_menu();
test_command_sequence<3> make_basic_test_command_sequence();

using test_shell = shell<test_state>;
using test_shell_command = typename shell<test_state>::command;
using test_shell_command_ptr = std::unique_ptr<test_shell_command>;

class test_shell_command1 : public test_shell_command {
    public:
        test_shell_command1() = default;
        virtual std::string name() const override;
        virtual std::string description() const override;
        virtual std::string help() const override;
        virtual code process_input(std::string_view command, test_state& state, streams& streams = default_streams) override;
};

class test_shell_command2 : public test_shell_command {
    public:
        test_shell_command2() = default;
        virtual std::string name() const override;
        virtual std::string description() const override;
        virtual std::string help() const override;
        virtual code process_input(std::string_view command, test_state& state, streams& streams = default_streams) override;
};

class test_shell_exit_command : public test_shell_command {
    public:
        test_shell_exit_command() = default;
        virtual std::string name() const override;
        virtual std::string description() const override;
        virtual std::string help() const override;
        virtual code process_input(std::string_view command, test_state& state, streams& streams = default_streams) override;
};

} // namespace tools::cli

#endif//TESTS_CLI_TEST_CLASSES_HPP