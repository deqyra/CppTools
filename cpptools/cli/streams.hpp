#ifndef CPPTOOLS__CLI__STREAMS_HPP
#define CPPTOOLS__CLI__STREAMS_HPP

#include <iostream>

namespace tools::cli
{

class streams
{
private:
    streams(const streams&) = delete;
    streams(streams&&) = delete;
    streams& operator=(const streams&) = delete;
    streams& operator=(streams&&) = delete;

public:
    std::istream& in  = std::cin;
    std::ostream& out = std::cout;
    std::ostream& err = std::cerr;

    streams() :
        in(std::cin),
        out(std::cout),
        err(std::cerr)
    {

    }

    streams(std::istream& in, std::ostream& out, std::ostream& err) :
        in(in),
        out(out),
        err(err)
    {

    }
};

} // namespace tools::cli

#endif//CPPTOOLS__CLI__STREAMS_HPP