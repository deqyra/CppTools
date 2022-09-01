#ifndef CPPTOOLS__EXCEPTION__IO_EXCEPTION_HPP
#define CPPTOOLS__EXCEPTION__IO_EXCEPTION_HPP

#include <string_view>

#include "exception.hpp"
#include "error_category.hpp"

namespace cpptools::exception
{

class io_exception : public base_exception
{
public:
    static constexpr error_category error_category = error_category::io;
    enum class error_code
    {
        file_not_found          = 0,
        access_denied           = 1,
        invalid_input_stream    = 2,
        invalid_output_stream   = 3,
    };

    io_exception(const std::string_view& stream_name);

    std::string to_string() const override;

private:
    std::string _stream_name;
};

template<>
constexpr std::string_view default_error_message(const io_exception::error_code& code);

template<>
constexpr std::string_view to_string(const io_exception::error_code& code);

namespace io
{
    using e = io_exception::error_code;

    using file_not_found_error = exception<io_exception, e::file_not_found>;
    using access_denied_error = exception<io_exception, e::access_denied>;
    using invalid_input_stream_error = exception<io_exception, e::invalid_input_stream>;
    using invalid_output_stream_error = exception<io_exception, e::invalid_output_stream>;
}

} // namespace cpptools::exception

#endif//CPPTOOLS__EXCEPTION__IO_EXCEPTION_HPP