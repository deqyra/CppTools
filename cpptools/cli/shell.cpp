#include "shell.hpp"

namespace tools::cli
{

std::string to_string(shell_command_code c)
{
    switch (c)
    {
        case shell_command_code::exit:
            return "exit";
        case shell_command_code::success:
            return "success";
        case shell_command_code::failure:
            return "failure";
        case shell_command_code::bad_arguments:
            return "bad_arguments";
        case shell_command_code::not_found:
            return "not_found";
        default:
            return "???";
    }
}

} // namespace tools::cli