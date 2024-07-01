#include "io_exception.hpp"
#include "exception.hpp"

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

std::string_view io_exception::to_string() const {
    static const std::string result =
        std::string(base_exception::to_string()) + '\n' +
        "Stream name: " + _stream_name;

    return result;
}

}
