#ifndef TESTS__CPPTOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP
#define TESTS__CPPTOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP

#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/micro_shell/micro_shell.hpp>
#include <cpptools/micro_shell/micro_shell_command.hpp>
#include <cpptools/micro_shell/micro_shell_codes.hpp>

namespace tools
{

class test_shell_state {};

using test_shell = micro_shell<test_shell_state>;
using test_shell_command = micro_shell_command<test_shell_state>;
using test_shell_command_ptr = std::unique_ptr<test_shell_command>;

class test_shell_command1 : public test_shell_command
{
    public:
        test_shell_command1() : test_shell_command() {}
        virtual std::string name() {return "test_shell_command1";}
        virtual std::string description() {return "Run test_shell_command1";}
        virtual std::string help() {return "test_shell_command1: no arguments expected.";}
        virtual int process_input(const std::string& command, test_shell_state& state, cli_streams& streams = cli_input::default_streams)
        {
            streams.out << "test_shell_command1 was run.\n";
            return status_code::command_success;
        }
};

class test_shell_command2 : public test_shell_command
{
    public:
        test_shell_command2() : test_shell_command() {}
        virtual std::string name() {return "test_shell_command2";}
        virtual std::string description() {return "Run test_shell_command2";}
        virtual std::string help() {return "test_shell_command2: no arguments expected.";}
        virtual int process_input(const std::string& command, test_shell_state& state, cli_streams& streams = cli_input::default_streams)
        {
            streams.out << "test_shell_command2 was run.\n";
            return status_code::command_success;
        }
};

class test_shell_exit_command : public test_shell_command
{
    public:
        test_shell_exit_command() : test_shell_command() {}
        virtual std::string name() {return "test_shell_exit_command";}
        virtual std::string description() {return "Run test_shell_exit_command";}
        virtual std::string help() {return "test_shell_exit_command: no arguments expected.";}
        virtual int process_input(const std::string& command, test_shell_state& state, cli_streams& streams = cli_input::default_streams)
        {
            streams.out << "test_shell_exit_command was run.\n";
            return status_code::exit;
        }
};

} // namespace tools

#endif//TESTS__CPPTOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP