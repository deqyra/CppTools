#ifndef CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP

#include <filesystem>
#include <string_view>

#include <cpptools/api.hpp>

#include "exception.hpp"
#include "error_category.hpp"

namespace tools::exception {

class io_exception : public base_exception {
public:
    static constexpr error_category error_category = error_category::io;
    enum class ecode {
        file_not_found          = 0,
        access_denied           = 1,
        invalid_input_stream    = 2,
        invalid_output_stream   = 3,
    };

    CPPTOOLS_API io_exception(std::string stream_name);
    CPPTOOLS_API io_exception(std::filesystem::path stream_path);

    CPPTOOLS_API std::string_view to_string() const override;

private:
    std::string _stream_name;
};

template<>
constexpr std::string_view default_error_message(const io_exception::ecode& code) {
    using e = io_exception::ecode;

    switch (code) {
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
constexpr std::string_view to_string(const io_exception::ecode& code) {
    using e = io_exception::ecode;

    switch (code) {
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

namespace io {
    using e = io_exception::ecode;

    using file_not_found_error = exception<io_exception, e::file_not_found>;
    using access_denied_error = exception<io_exception, e::access_denied>;
    using invalid_input_stream_error = exception<io_exception, e::invalid_input_stream>;
    using invalid_output_stream_error = exception<io_exception, e::invalid_output_stream>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP