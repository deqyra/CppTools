#include "cli_streams.hpp"

#include <iostream>

CLIStreams::CLIStreams(std::istream& in, std::ostream& out, std::ostream& err) :
    _in(in),
    _out(out),
    _err(err)
{

}

std::istream& CLIStreams::in()
{
    return _in;
}

std::ostream& CLIStreams::out()
{
    return _out;
}

std::ostream& CLIStreams::err()
{
    return _err;
}
