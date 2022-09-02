#include "cli_test_classes.hpp"

#include <cpptools/cli/cli_menu.hpp>
#include <cpptools/cli/cli_command.hpp>
#include <cpptools/cli/cli_streams.hpp>
#include <cpptools/cli/cli_input.hpp>
#include <cpptools/cli/menu_command.hpp>
#include <cpptools/cli/command_sequence.hpp>

namespace tools
{

Testcli_menu make_basic_test_menu()
{
    std::vector<TestCLIcommand_ptr> commands = {
        std::make_shared<Testcli_command1>(),
        std::make_shared<Testcli_command2>()
    };

    return Testcli_menu(commands, "Test menu", "Exit", std::make_shared<TestCLIexit_command>());
}

TestCLIcommand_sequence make_basic_test_command_sequence()
{
    Testcli_menu test_menu = make_basic_test_menu();

    std::vector<TestCLIcommand_ptr> sequence_commands = {
        std::make_shared<TestCLImenu_command>(test_menu),
        std::make_shared<Testcli_command1>(),
        std::make_shared<Testcli_command2>()
    };

    return TestCLIcommand_sequence(sequence_commands, "Test sequence");
}

} // namespace tools
