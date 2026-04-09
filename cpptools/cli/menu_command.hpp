#ifndef CPPTOOLS_CLI_MENU_COMMAND_HPP
#define CPPTOOLS_CLI_MENU_COMMAND_HPP

#include <string_view>

#include "command.hpp"
#include "menu.hpp"

namespace tools::cli {

// Extended command wrapping a menu.
template<cli_environment env_t>
class menu_command : public command<env_t> {
public:
    using code = typename command<env_t>::code;

private:
    menu<env_t> _inner_menu;

public: 
    menu_command(menu<env_t>&& inner_menu) :
        command<env_t>(),
        _inner_menu(std::move(inner_menu))
    {

    }

    menu_command(menu_command&& other) = default;

    virtual ~menu_command() = default;

    virtual std::string_view tooltip() const override {
        return _inner_menu.tooltip();
    }

    // Run the menu_command (show the inner menu).
    virtual code run(env_t& env) override {
        _inner_menu.show(env);
        return code::success;
    }
};

} // namespace tools::cli

#endif//CPPTOOLS_CLI_MENU_COMMAND_HPP