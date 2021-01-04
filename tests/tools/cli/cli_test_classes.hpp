#ifndef TESTS__TOOLS__CLI__CLI_TEST_CLASSES_HPP
#define TESTS__TOOLS__CLI__CLI_TEST_CLASSES_HPP

#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_command.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

namespace CppTools
{

class TestCLIState {};

using TestCLIMenu = CLIMenu<TestCLIState>;
using TestCLICommand = CLICommand<TestCLIState>;
using TestCLICommandPtr = std::shared_ptr<TestCLICommand>;
using TestCLIMenuCommand = MenuCommand<TestCLIState>;
using TestCLICommandSequence = CommandSequence<TestCLIState>;

class TestCLICommand1 : public TestCLICommand
{
    public:
        TestCLICommand1() : TestCLICommand() {}
        virtual std::string getTooltip() {return "TestCLICommand1";}
        virtual int run(TestCLIState& state, CLIStreams& streams)
        {
            streams.out() << "TestCLICommand1 was run.\n";
            return 0;
        }
};

class TestCLICommand2 : public TestCLICommand
{
    public:
        TestCLICommand2() : TestCLICommand() {}
        virtual std::string getTooltip() {return "TestCLICommand2";}
        virtual int run(TestCLIState& state, CLIStreams& streams)
        {
            streams.out() << "TestCLICommand2 was run.\n";
            return 0;
        }
};

class TestCLIExitCommand : public TestCLICommand
{
    public:
        TestCLIExitCommand() : TestCLICommand() {}
        virtual std::string getTooltip() {return "Exit";}
        virtual int run(TestCLIState& state, CLIStreams& streams)
        {
            streams.out() << "Successful exit.\n";
            return 0;
        }
};

TestCLIMenu makeBasicTestMenu();
TestCLICommandSequence makeBasicTestCommandSequence();

}//namespace CppTools

#endif//TESTS__TOOLS__CLI__CLI_TEST_CLASSES_HPP