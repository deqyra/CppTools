#ifndef TESTS__CPPTOOLS__CLI__CLI_TEST_CLASSES_HPP
#define TESTS__CPPTOOLS__CLI__CLI_TEST_CLASSES_HPP

#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_command.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

namespace tools
{

class TestCLIState {};

using Testcli_menu = cli_menu<TestCLIState>;
using Testcli_command = cli_command<TestCLIState>;
using TestCLIcommand_ptr = std::unique_ptr<Testcli_command>;
using TestCLImenu_command = menu_command<TestCLIState>;
using TestCLIcommand_sequence = command_sequence<TestCLIState>;

class Testcli_command1 : public Testcli_command
{
    public:
        Testcli_command1() : Testcli_command() {}
        virtual std::string get_tooltip() {return "Testcli_command1";}
        virtual int run(TestCLIState& state, cli_streams& streams)
        {
            streams.out << "Testcli_command1 was run.\n";
            return 0;
        }
};

class Testcli_command2 : public Testcli_command
{
    public:
        Testcli_command2() : Testcli_command() {}
        virtual std::string get_tooltip() {return "Testcli_command2";}
        virtual int run(TestCLIState& state, cli_streams& streams)
        {
            streams.out << "Testcli_command2 was run.\n";
            return 0;
        }
};

class TestCLIexit_command : public Testcli_command
{
    public:
        TestCLIexit_command() : Testcli_command() {}
        virtual std::string get_tooltip() {return "Exit";}
        virtual int run(TestCLIState& state, cli_streams& streams)
        {
            streams.out << "Successful exit.\n";
            return 0;
        }
};

Testcli_menu make_basic_test_menu();
TestCLIcommand_sequence make_basic_test_command_sequence();

} // namespace tools

#endif//TESTS__CPPTOOLS__CLI__CLI_TEST_CLASSES_HPP