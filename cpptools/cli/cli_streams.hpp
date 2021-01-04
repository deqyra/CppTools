#ifndef CPPTOOLS__CLI__CLI_STREAMS_HPP
#define CPPTOOLS__CLI__CLI_STREAMS_HPP

#include <iostream>

namespace CppTools
{

class CLIStreams
{
    private:    // Attributes
        std::istream& _in;
        std::ostream& _out;
        std::ostream& _err;

    public:     // Public methods
        CLIStreams(std::istream& in = std::cin, std::ostream& out = std::cout, std::ostream& err = std::cerr);

        std::istream& in();
        std::ostream& out();
        std::ostream& err();
};

}//namespace CppTools

#endif//CPPTOOLS__CLI__CLI_STREAMS_HPP