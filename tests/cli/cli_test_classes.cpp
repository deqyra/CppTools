#include "cli_test_classes.hpp"

#include <tools/cli/cli_menu.hpp>
#include <tools/cli/cli_command.hpp>
#include <tools/cli/cli_streams.hpp>
#include <tools/cli/cli_input.hpp>
#include <tools/cli/menu_command.hpp>
#include <tools/cli/command_sequence.hpp>

namespace tools
{

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

} // namespace tools
