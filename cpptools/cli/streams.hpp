#ifndef CPPTOOLS__CLI__STREAMS_HPP
#define CPPTOOLS__CLI__STREAMS_HPP

#include <iostream>

namespace tools::cli
{

class streams
{
public:
    std::istream& in  = std::cin;
    std::ostream& out = std::cout;
    std::ostream& err = std::cerr;
};

} // namespace tools::cli

#endif//CPPTOOLS__CLI__STREAMS_HPP