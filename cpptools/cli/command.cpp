#include "command.hpp"

namespace tools::cli
{

std::string to_string(cli_command_code c)
{
    switch (c)
    {
        case cli_command_code::success:
            return "success";
        case cli_command_code::failure:
            return "failure";
        default:
            return "???";
    }
}

} // namespace tools::cli