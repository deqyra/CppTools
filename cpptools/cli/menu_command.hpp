#ifndef CPPTOOLS__CLI__MENU_COMMAND_HPP
#define CPPTOOLS__CLI__MENU_COMMAND_HPP

#include <array>
#include <string>

#include "command.hpp"
#include "menu.hpp"
#include "streams.hpp"

namespace tools::cli
{

// Extended command wrapping a menu.
template<typename context_t, std::size_t N>
class menu_command : public command<context_t>
{
public:
    using code = typename command<context_t>::code;

private:
    menu<context_t, N> _inner_menu;

public: 
    menu_command(menu<context_t, N>&& inner_menu) :
        command<context_t>(),
        _inner_menu(std::move(inner_menu))
    {

    }

    menu_command(menu_command&& other) = default;

    virtual ~menu_command() = default;

    virtual std::string tooltip() const override
    {
        return _inner_menu.tooltip();
    }

    // Run the menu_command (show the inner menu).
    virtual code run(context_t& state, cli::streams& streams) override
    {
        _inner_menu.show(state, streams);
        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS__CLI__MENU_COMMAND_HPP