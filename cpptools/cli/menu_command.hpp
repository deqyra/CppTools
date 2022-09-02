#ifndef CPPTOOLS__CLI__MENU_COMMAND_HPP
#define CPPTOOLS__CLI__MENU_COMMAND_HPP

#include <string>

#include "cli_command.hpp"
#include "cli_menu.hpp"
#include "cli_streams.hpp"

namespace tools
{

// Extended command wrapping a menu.
template<typename state_t>
class menu_command : public cli_command<state_t>
{
public:
    using code = typename cli_command<state_t>::code;

private:
    cli_menu<state_t> _inner_menu;

public: 
    menu_command(cli_menu<state_t> inner_menu) :
        cli_command<state_t>(),
        _inner_menu(inner_menu)
    {

    }

    virtual ~menu_command() = default;

    virtual std::string get_tooltip()
    {
        return _inner_menu.get_tooltip();
    }

    // Run the menu_command (show the inner menu).
    virtual int run(state_t& state, cli_streams& streams)
    {
        _inner_menu.show(state, streams);
        return code::success;
    }
};

} // namespace tools

#endif//CPPTOOLS__CLI__MENU_COMMAND_HPP