#include "io_exception.hpp"
#include "exception.hpp"

#include <sstream>

namespace tools::exception
{

io_exception::io_exception(const std::string_view& stream_name) :
    base_exception(),
    _stream_name(stream_name)
{

}

std::string io_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << base_exception::error_code() << ") " << error_code_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Stream name: " << _stream_name << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

template<>
constexpr std::string_view default_error_message(const io_exception::error_code& code)
{
    using e = io_exception::error_code;

    switch (code)
    {
    case e::file_not_found:
        return "The file could not be found";
    case e::access_denied:
        return "Access to the file was denied";
    case e::invalid_input_stream:
        return "Invalid input stream";
    case e::invalid_output_stream:
        return "Invalid output stream";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const io_exception::error_code& code)
{
    using e = io_exception::error_code;

    switch (code)
    {
    case e::file_not_found:
        return "file_not_found";
    case e::access_denied:
        return "access_denied";
    case e::invalid_input_stream:
        return "invalid_input_stream";
    case e::invalid_output_stream:
        return "invalid_output_stream";

    default:
        return "???";
    }
}

}
