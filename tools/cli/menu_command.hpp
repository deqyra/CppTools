#ifndef TOOLS__CLI__MENU_COMMAND_HPP
#define TOOLS__CLI__MENU_COMMAND_HPP

#include "cli_command.hpp"

#include <string>

#include "cli_menu.hpp"
#include "cli_streams.hpp"

// Extended command wrapping a menu.
template<typename CustomState>
class MenuCommand : public CLICommand<CustomState>
{
    private:    // Attributes
        CLIMenu<CustomState> _innerMenu;

    public:     // Public methods
        MenuCommand(CLIMenu<CustomState> innerMenu);
        virtual ~MenuCommand();

        virtual std::string getTooltip();
        // Run the MenuCommand (show the inner menu).
        virtual int run(CustomState& state, CLIStreams& streams);
};

template<typename CustomState>
MenuCommand<CustomState>::MenuCommand(CLIMenu<CustomState> innerMenu) :
    CLICommand<CustomState>(),
    _innerMenu(innerMenu)
{

}

template<typename CustomState>
MenuCommand<CustomState>::~MenuCommand()
{
    
}

template<typename CustomState>
std::string MenuCommand<CustomState>::getTooltip()
{
    return _innerMenu.getTooltip();
}

template<typename CustomState>
int MenuCommand<CustomState>::run(CustomState& state, CLIStreams& streams)
{
    _innerMenu.show(state, streams);
    return CLI_COMMAND_SUCCESS;
}

#endif//TOOLS__CLI__MENU_COMMAND_HPP