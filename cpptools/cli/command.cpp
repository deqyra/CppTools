#include "command.hpp"

namespace tools::cli
{

std::string to_string(command_code c)
{
    switch (c)
    {
        case command_code::success:
            return "success";
        case command_code::failure:
            return "failure";
        default:
            return "???";
    }
}

} // namespace tools::cli