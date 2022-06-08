#ifndef TESTS__TOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP
#define TESTS__TOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP

#include <tools/cli/cli_streams.hpp>
#include <tools/cli/cli_input.hpp>
#include <tools/micro_shell/micro_shell.hpp>
#include <tools/micro_shell/micro_shell_command.hpp>
#include <tools/micro_shell/micro_shell_codes.hpp>

namespace tools
{

class TestShellState {};

using TestShell = MicroShell<TestShellState>;
using TestShellCommand = MicroShellCommand<TestShellState>;
using TestShellCommandPtr = std::shared_ptr<TestShellCommand>;

class TestShellCommand1 : public TestShellCommand
{
    public:
        TestShellCommand1() : TestShellCommand() {}
        virtual std::string name() {return "TestShellCommand1";}
        virtual std::string description() {return "Run TestShellCommand1";}
        virtual std::string help() {return "TestShellCommand1: no arguments expected.";}
        virtual int processInput(const std::string& command, TestShellState& state, CLIStreams& streams = CLIInput::defaultStreams)
        {
            streams.out() << "TestShellCommand1 was run.\n";
            return SHELL_COMMAND_SUCCESS;
        }
};
 
class TestShellCommand2 : public TestShellCommand
{
    public:
        TestShellCommand2() : TestShellCommand() {}
        virtual std::string name() {return "TestShellCommand2";}
        virtual std::string description() {return "Run TestShellCommand2";}
        virtual std::string help() {return "TestShellCommand2: no arguments expected.";}
        virtual int processInput(const std::string& command, TestShellState& state, CLIStreams& streams = CLIInput::defaultStreams)
        {
            streams.out() << "TestShellCommand2 was run.\n";
            return SHELL_COMMAND_SUCCESS;
        }
};

class TestShellExitCommand : public TestShellCommand
{
    public:
        TestShellExitCommand() : TestShellCommand() {}
        virtual std::string name() {return "TestShellExitCommand";}
        virtual std::string description() {return "Run TestShellExitCommand";}
        virtual std::string help() {return "TestShellExitCommand: no arguments expected.";}
        virtual int processInput(const std::string& command, TestShellState& state, CLIStreams& streams = CLIInput::defaultStreams)
        {
            streams.out() << "TestShellExitCommand was run.\n";
            return SHELL_EXIT;
        }
};

} // namespace tools

#endif//TESTS__TOOLS__MICRO_SHELL__SHELL_TEST_CLASSES_HPP