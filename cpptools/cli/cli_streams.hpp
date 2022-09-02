#ifndef CPPTOOLS__CLI__CLI_STREAMS_HPP
#define CPPTOOLS__CLI__CLI_STREAMS_HPP

#include <iostream>

namespace tools
{

class cli_streams
{
public:
    std::istream& in  = std::cin;
    std::ostream& out = std::cout;
    std::ostream& err = std::cerr;
};

} // namespace tools

#endif//CPPTOOLS__CLI__CLI_STREAMS_HPP