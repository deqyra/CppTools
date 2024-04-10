#ifndef CPPTOOLS_CLI_MENU_COMMAND_HPP
#define CPPTOOLS_CLI_MENU_COMMAND_HPP

#include <string_view>

#include "command.hpp"
#include "menu.hpp"
#include "streams.hpp"

namespace tools::cli {

// Extended command wrapping a menu.
template<typename Context, std::size_t N>
class menu_command : public command<Context> {
public:
    using code = typename command<Context>::code;

private:
    menu<Context, N> _inner_menu;

public: 
    menu_command(menu<Context, N>&& inner_menu) :
        command<Context>(),
        _inner_menu(std::move(inner_menu))
    {

    }

    menu_command(menu_command&& other) = default;

    virtual ~menu_command() = default;

    virtual std::string_view tooltip() const override {
        return _inner_menu.tooltip();
    }

    // Run the menu_command (show the inner menu).
    virtual code run(Context& state, cli::streams& streams) override {
        _inner_menu.show(state, streams);
        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_MENU_COMMAND_HPP