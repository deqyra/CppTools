#ifndef CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP
#define CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP

#include <filesystem>
#include <string_view>

#include <cpptools/api.hpp>

#include "exception.hpp"
#include "error_category_t.hpp"

namespace tools::exception {

class io_exception : public base_exception<error_category_t> {
public:
    using error_category_t = tools::exception::error_category_t;
    static constexpr error_category_t error_category = error_category_t::io;

    enum class error_code_t {
        file_not_found          = 0,
        access_denied           = 1,
        invalid_input_stream    = 2,
        invalid_output_stream   = 3,
    };

    CPPTOOLS_API io_exception(std::string stream_name) :
        base_exception(),
        _stream_name(stream_name)
    {

    }

    CPPTOOLS_API io_exception(std::filesystem::path stream_path) :
        base_exception(),
        _stream_name(stream_path.string())
    {

    }

    CPPTOOLS_API std::string_view to_string() const override {
        base_exception::_str = std::string(base_exception::to_string()) + '\n' + "Stream name: " + _stream_name;

        return base_exception::_str;
    }

    CPPTOOLS_API std::string_view stream_name() const {
        return _stream_name;
    }

private:
    std::string _stream_name;
};

template<>
constexpr std::string_view default_error_message(const io_exception::error_code_t& code) {
    using enum io_exception::error_code_t;

    switch (code) {
    case file_not_found:
        return "The file could not be found";
    case access_denied:
        return "Access to the file was denied";
    case invalid_input_stream:
        return "Invalid input stream";
    case invalid_output_stream:
        return "Invalid output stream";

    default:
        return "???";
    }
}

template<>
constexpr std::string_view to_string(const io_exception::error_code_t& code) {
    using enum io_exception::error_code_t;

    switch (code) {
    case file_not_found:
        return "file_not_found";
    case access_denied:
        return "access_denied";
    case invalid_input_stream:
        return "invalid_input_stream";
    case invalid_output_stream:
        return "invalid_output_stream";

    default:
        return "???";
    }
}

static_assert(concrete_exception<io_exception>);

namespace io {
    using enum io_exception::error_code_t;

    using file_not_found_error        = exception<io_exception, file_not_found>;
    using access_denied_error         = exception<io_exception, access_denied>;
    using invalid_input_stream_error  = exception<io_exception, invalid_input_stream>;
    using invalid_output_stream_error = exception<io_exception, invalid_output_stream>;
}

} // namespace tools::exception

#endif//CPPTOOLS_EXCEPTION_IO_EXCEPTION_HPP