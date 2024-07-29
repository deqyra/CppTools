#ifndef CPPTOOLS_CLI_STREAMS_HPP
#define CPPTOOLS_CLI_STREAMS_HPP

#include <iostream>
#include <ostream>

namespace tools::cli {

template<typename Char>
class basic_no_ostream : public std::basic_ostream<Char> {
public:
    basic_no_ostream() :
        std::basic_ostream<Char>(nullptr)
    {

    }

    template<typename T>
    basic_no_ostream& operator<<(T&& arg) { return *this; }
};

using no_ostream = basic_no_ostream<char>;
using no_wostream = basic_no_ostream<wchar_t>;

template<typename Char>
inline auto basic_no_out = basic_no_ostream<Char>{};

inline auto& no_out = basic_no_out<char>;
inline auto& no_wout = basic_no_out<wchar_t>;

template<typename Char>
class basic_no_istream : public std::basic_istream<Char> {
public:
    basic_no_istream() :
        std::basic_istream<Char>(nullptr)
    {

    }

    template<typename T>
    basic_no_istream& operator>>(T&& arg) { return *this; }
};

using no_istream = basic_no_istream<char>;
using no_wistream = basic_no_istream<wchar_t>;

template<typename Char>
inline auto basic_no_in = basic_no_istream<Char>{};

inline auto& no_in = basic_no_in<char>;
inline auto& no_win = basic_no_in<wchar_t>;

template<typename Char>
class basic_streams {
private:
    basic_streams(const basic_streams&) = default;
    basic_streams(basic_streams&&) = default;
    basic_streams& operator=(const basic_streams&) = delete;
    basic_streams& operator=(basic_streams&&) = delete;

public:
    std::basic_istream<Char>& in;
    std::basic_ostream<Char>& out;
    std::basic_ostream<Char>& err;

    basic_streams(std::basic_istream<Char>& in, std::basic_ostream<Char>& out, std::basic_ostream<Char>& err) :
        in(in),
        out(out),
        err(err)
    {

    }
};

using streams = basic_streams<char>;
using wstreams = basic_streams<wchar_t>;

inline auto no_streams = streams(no_in, no_out, no_out);
inline auto no_wstreams = wstreams(no_win, no_wout, no_wout);

inline auto default_streams = streams(std::cin, std::cout, std::cerr);
inline auto default_wstreams = wstreams(std::wcin, std::wcout, std::wcerr);

} // namespace tools::cli

#endif//CPPTOOLS_CLI_STREAMS_HPP