#ifndef TOOLS__PARSE__TOKEN_HPP
#define TOOLS__PARSE__TOKEN_HPP

#include <string>

template<typename T>
struct Token
{
    const T type;
    std::string value;
    unsigned int startPos;
    unsigned int endPos;
    unsigned int line;
    unsigned int column;
    Token<T>* matchingTokenForward;
    Token<T>* matchingTokenBackward;
};

#endif//TOOLS__PARSE__TOKEN_HPP