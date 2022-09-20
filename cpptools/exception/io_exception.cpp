#include "io_exception.hpp"
#include "exception.hpp"

#include <sstream>

namespace tools::exception
{

io_exception::io_exception(std::string stream_name) :
    base_exception(),
    _stream_name(stream_name)
{

}

io_exception::io_exception(std::filesystem::path stream_path) :
    base_exception(),
    _stream_name(stream_path.string())
{

}

std::string io_exception::to_string() const
{
    std::stringstream result;
    result << "Category: " << error_category_name(category())
           << ", error: (" << error_code() << ") " << ecode_to_string() << '\n';
    result << "Function <" << function() << ">, line: " << line() << '\n';
    result << "Stream name: " << _stream_name << '\n';
    result << "Message: " << message() << '\n';

    return result.str();
}

}
