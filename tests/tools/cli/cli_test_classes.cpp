#include "cli_test_classes.hpp"

#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_command.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

TestCLIMenu makeBasicTestMenu()
{
    std::vector<TestCLICommandPtr> commands = {
        std::make_shared<TestCLICommand1>(),
        std::make_shared<TestCLICommand2>()
    };

    return TestCLIMenu(commands, "Test menu", "Exit", std::make_shared<TestCLIExitCommand>());
}

TestCLICommandSequence makeBasicTestCommandSequence()
{
    TestCLIMenu testMenu = makeBasicTestMenu();
    
    std::vector<TestCLICommandPtr> sequenceCommands = {
        std::make_shared<TestCLIMenuCommand>(testMenu),
        std::make_shared<TestCLICommand1>(),
        std::make_shared<TestCLICommand2>()
    };

    return TestCLICommandSequence(sequenceCommands, "Test sequence");
}
