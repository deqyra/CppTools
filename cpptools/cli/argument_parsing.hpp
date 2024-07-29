#ifndef CPPTOOLS_CLI_ARGUMENT_PARSER_HPP
#define CPPTOOLS_CLI_ARGUMENT_PARSER_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <map>
#include <ostream>
#include <span>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <cpptools/cli/streams.hpp>
#include <cpptools/exception/arg_parse_exception.hpp>
#include <cpptools/exception/parameter_exception.hpp>
#include <cpptools/exception/lookup_exception.hpp>
#include <cpptools/utility/ranges.hpp>
#include <cpptools/utility/string.hpp>
#include <cpptools/utility/to_string.hpp>

namespace tools::cli {

enum class necessity {
    optional,
    required
};

enum class multiplicity {
    once,
    many
};

template<typename Char>
struct basic_argument_name {
    /// @brief Long name of the argument (specify "argument_name" for --argument_name, "" for no long name)
    std::basic_string<Char> long_name = "";
    /// @brief Short name of the argument (specify 'a' for -a, '\0' for no short name)
    Char short_name = '\0';

    std::strong_ordering operator<=>(const basic_argument_name<Char>&) const = default;
};

namespace detail {

    std::string inline compose_arg_name_string(std::string&& long_name, std::string&& short_name) {
        using namespace std::string_literals;
        std::string result;
        
        if (long_name.empty()) {
            result = "\"" + std::move(long_name) + "\"";
        }

        if (short_name.empty()) {
            if (!result.empty()) {
                result += " / ";
            }
            result += "'"s + std::move(short_name) + "'";
        }

        return result;
    }

}

template<typename Char>
std::string to_string(const basic_argument_name<Char>& arg_name) {
    return detail::compose_arg_name_string(tools::to_string(arg_name.long_name), tools::to_string(arg_name.short_name));
}

template<typename Char>
struct basic_argument {
    /// @brief Name of the argument, in short or long form, or both forms
    basic_argument_name<Char> name;
    /// @brief Whether the argument is required or not
    necessity necessity;
    /// @brief The number of values to be provided immediately after the argument. -1 consumes the rest of the arguments
    int value_count;
};

namespace detail
{

template<typename Char>
auto long_name_matches(std::basic_string_view<Char> long_name) noexcept {
    return [long_name](const basic_argument<Char>& arg_spec) {
        return arg_spec.name.long_name == long_name;
    };
};

template<typename Char>
auto short_name_matches(Char short_name) noexcept {
    return [short_name](const basic_argument<Char>& arg_spec) {
        return arg_spec.name.short_name == short_name;
    };
};

template<typename Char>
void maybe_suggest_long_arg(std::basic_string_view<Char> arg_value, const std::vector<basic_argument<Char>>& args_spec, std::basic_ostream<Char>& out) {
    if (std::ranges::find_if(args_spec, long_name_matches(arg_value)) != args_spec.end()) {
        out << " Did you mean to write \"--" << arg_value << "\"?";
    }
}

template<typename Char>
void maybe_suggest_short_arg(Char arg_value, const std::vector<basic_argument<Char>>& arg_spec, std::basic_ostream<Char>& out) {
    if (std::ranges::find_if(arg_spec, short_name_matches(arg_value)) != arg_spec.end()) {
        out << " Did you mean to write \"-" << arg_value << "\"?";
    }
}

template<typename Char>
void validate_arg_specs(const std::vector<basic_argument<Char>>& arg_specs) {
    const basic_argument<Char>* first_consuming_parameter = nullptr;
    std::set<Char> short_names;
    std::set<std::basic_string_view<Char>> long_names;

    for (const auto& spec : arg_specs) {
        const auto& [long_n, short_n] = spec.name;

        if (long_n == "" && short_n == '\0') {
            CPPTOOLS_THROW(exception::arg_parse::param_with_no_name_error, "");
        }

        if (long_n != "") {
            if (long_names.contains(long_n)) {
                CPPTOOLS_THROW(exception::arg_parse::multiple_params_with_same_name_error, long_n);
            }
            long_names.insert(long_n);
        } 

        if (short_n != '\0') {
            if (short_names.contains(short_n)) {
                CPPTOOLS_THROW(exception::arg_parse::multiple_params_with_same_name_error, std::to_string(short_n));
            }
            short_names.insert(short_n);
        }

        if (spec.value_count == -1) {
            if (first_consuming_parameter == nullptr) {
                first_consuming_parameter = &spec;
            } else {
                CPPTOOLS_THROW(
                    exception::arg_parse::multiple_consume_remaining_args_error,
                    to_string(first_consuming_parameter->name)
                ).with_message("Second parameter: " + to_string(spec.name));
            }
        }
    }
}

template<typename Char>
auto handle_long_arg(std::basic_string_view<Char> arg, const std::vector<basic_argument<Char>>& arg_specs, std::basic_ostream<Char>& out) {
    auto result = std::ranges::find_if(arg_specs, long_name_matches(arg));
    if (result == arg_specs.end()) {
        out << "Warning: unknown argument \"--" << arg << "\" will be ignored.";
        if (arg.size() == 1) {
            detail::maybe_suggest_short_arg(arg.at(0), arg_specs, out);
        }
        out << '\n';
    }

    return result;
}

template<typename Char>
auto handle_short_arg(std::basic_string_view<Char> arg, const std::vector<basic_argument<Char>>& arg_specs, std::basic_ostream<Char>& out) {
    auto result = arg_specs.end();
    if (arg.size() == 1) {
        result = std::ranges::find_if(arg_specs, short_name_matches(arg.at(0)));
    }

    if (result == arg_specs.end()) {
        out << "Warning: unknown argument \"-" << arg << "\" will be ignored.";
        detail::maybe_suggest_long_arg(arg, arg_specs, out);
        out << '\n';
    }

    return result;
}

template<typename Char>
void handle_bad_arg(std::basic_string_view<Char> arg, const std::vector<basic_argument<Char>>& arg_specs, std::basic_ostream<Char>& out) {
    out << "Warning: badly formatted argument \"" << arg << "\" will be ignored.";
    detail::maybe_suggest_long_arg(arg, arg_specs, out);
    if (arg.size() == 1) {
        detail::maybe_suggest_short_arg(arg.at(0), arg_specs, out);
    }
    out << '\n';
}

} // namespace detail

template<typename Char>
class basic_argument_value_map {
public:
    class value_vec {
        using underlying_t = std::vector<std::basic_string<Char>>;
        underlying_t _values;

    public:
        constexpr value_vec()                            = default;
        constexpr value_vec(const value_vec&)            = default;
        constexpr value_vec(value_vec&&)                 = default;
        constexpr value_vec& operator=(const value_vec&) = default;
        constexpr value_vec& operator=(value_vec&&)      = default;

        constexpr value_vec(underlying_t values)
            : _values(std::move(values))
        {

        }

        friend auto operator<=>(const value_vec& left, const value_vec& right) = default;

        value_vec& operator=(std::basic_string<Char> other) {
            _values.clear();
            _values.push_back(std::move(other));
            return *this;
        }

        [[nodiscard]] constexpr auto begin(this auto&& self) noexcept {
            return self._values.begin();
        }

        [[nodiscard]] constexpr auto end(this auto&& self) noexcept {
            return self._values.begin();
        }

        [[nodiscard]] constexpr auto cbegin() const noexcept {
            return _values.begin();
        }

        [[nodiscard]] constexpr auto cend() const noexcept {
            return _values.begin();
        }

        constexpr void push_back(std::basic_string<Char> item) {
            _values.push_back(std::move(item));
        }

        template<typename... Args>
        constexpr auto emplace_back(Args&&... args) {
            return _values.emplace_back(std::forward<Args>(args)...);
        }

        constexpr void clear() noexcept {
            _values.clear();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return _values.empty();
        }

        [[nodiscard]] constexpr bool operator==(std::basic_string_view<Char> other) const {
            return _values.size() == 1 && _values.at(0) == other;
        }

        template<std::ranges::input_range Rng>
            requires (std::equality_comparable_with<std::ranges::range_value_t<Rng>, std::basic_string_view<Char>>)
        [[nodiscard]] constexpr bool operator==(const Rng& other) const {
            return std::ranges::equal(_values, other);
        }
    };

private:
    using key_t = basic_argument_name<Char>;
    std::map<key_t, value_vec> _values;

    [[nodiscard]] constexpr auto _find_short_name(Char c) const {
        if (c == '\0') {
            CPPTOOLS_THROW(exception::parameter::null_parameter_error, "c");
        }

        auto it = std::ranges::find(_values | std::views::keys | views::member(&key_t::short_name), c);
        return it   // member_view_iterator
            .base() // keys_view_iterator
            .base();// map::iterator
    }

    [[nodiscard]] constexpr auto _find_long_name(std::basic_string_view<Char> s) const {
        if (s == "") {
            CPPTOOLS_THROW(exception::parameter::null_parameter_error, "s");
        }

        auto it = std::ranges::find(_values | std::views::keys | views::member(&key_t::long_name), s);
        return it   // member_view_iterator
            .base() // keys_view_iterator
            .base();// map::iterator
    }

    [[nodiscard]] constexpr auto _find_name(basic_argument_name<Char> n) const {
        if (n == basic_argument_name<Char>{}) {
            CPPTOOLS_THROW(exception::parameter::null_parameter_error, "n");
        }

        auto it = _values.end(); // cannot default-initialize `it` or subsequent comparisons fail because debug MSVC stdlib complains that it belongs to a different map than .end()
        if (n.short_name != '\0') {
            it = _find_short_name(n.short_name);
        }
        if (it == _values.end() && n.long_name != "") {
            it = _find_long_name(n.long_name);
        }

        return it;
    }

public:
    constexpr basic_argument_value_map() = default;

    constexpr basic_argument_value_map(std::map<key_t, value_vec> values) :
        _values(std::move(values))
    {
        
    }

    [[nodiscard]] constexpr decltype(auto) operator[](this auto&& self, Char c) {
        auto it = self._find_short_name(c);
        if (it == self._values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, c);
        }

        return it->second;
    }

    [[nodiscard]] constexpr decltype(auto) operator[](this auto&& self, std::basic_string_view<Char> s) {
        auto it = self._find_long_name(s);
        if (it == self._values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, s);
        }

        return it->second;
    }

    [[nodiscard]] constexpr decltype(auto) operator[](this auto&& self, const basic_argument_name<Char>& n) {
        auto it = self._find_name(n);
        if (it == self._values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, n);
        }

        return it->second;
    }

    [[nodiscard]] constexpr bool has(Char c) const {
        return _find_short_name(c) != _values.end(); // intentionally throws on '\0'
    }

    [[nodiscard]] constexpr bool has(std::basic_string_view<Char> s) const {
        return _find_long_name(s) != _values.end();  // intentionally throws on ""
    }

    [[nodiscard]] constexpr bool has(const basic_argument_name<Char>& n) const {
        return _find_name(n) != _values.end();
    }

    constexpr void erase(Char c) {
        auto it = _find_short_name(c);
        if (it == _values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, c);
        }

        _values.erase(it);
    }

    constexpr void erase(std::basic_string_view<Char> s) {
        auto it = _find_long_name(s);
        if (it == _values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, s);
        }

        _values.erase(it);
    }

    constexpr void erase(const key_t& n) const {
        auto it = _find_name(n);

        if (it == _values.end()) {
            CPPTOOLS_THROW(exception::lookup::no_such_element_error, to_string(n));
        }

        return _values.erase(it);
    }

    constexpr void insert(key_t name, value_vec value) {
        _values.insert_or_assign(std::move(name), std::move(value));
    }
};

template<typename Char>
basic_argument_value_map<Char> parse_arguments(
    int argc, const Char** argv,
    std::basic_ostream<Char>& out = cli::basic_no_out<Char>
)
{
    namespace stdv = std::views;
    using detail::long_name_matches;
    using detail::short_name_matches;

    auto argv_view = std::span<const Char*>(argv, argc)
        | stdv::drop(1)
        | stdv::transform([](const Char* a){ return std::basic_string_view<Char>(a); });

    basic_argument_value_map<Char> result;

    auto it = argv_view.begin();
    while (it != argv_view.end()) {
        auto this_arg = *it;
        ++it;

        basic_argument_name<Char> name;

        if (this_arg.starts_with("--")) {
            this_arg.remove_prefix(2);
            name.long_name = std::basic_string<Char>(this_arg);
        } else if (this_arg.starts_with("-")) {
            this_arg.remove_prefix(1);
            name.short_name = this_arg.at(0);
        } else {
            out << "Warning: argument name \"" << this_arg << "\" without dashes will be ignored.";
            continue;
        }

        using value_vec = std::vector<std::basic_string<Char>>;
        value_vec this_arg_values;

        while (it != argv_view.end()) {
            auto value = *it;
            if (value.starts_with("--") || value.starts_with("-")) {
                break;
            }
            
            this_arg_values.push_back(std::basic_string<Char>(value));
            ++it;
        }

        result.insert(std::move(name), std::move(this_arg_values));
    }

    return result;
}

template<typename Char>
basic_argument_value_map<Char> parse_arguments(
    int argc, const Char** argv,
    const std::vector<basic_argument<Char>>& arg_specs,
    std::basic_ostream<Char>& out = cli::basic_no_out<Char>
) {
    namespace stdv = std::views;
    using detail::long_name_matches;
    using detail::short_name_matches;

    detail::validate_arg_specs(arg_specs);

    auto argv_view = std::span<const Char*>(argv, argc)
        | stdv::drop(1)
        | stdv::transform([](const Char* a){ return std::basic_string_view<Char>(a); });

    basic_argument_value_map<Char> result;

    auto it = argv_view.begin();
    while (it != argv_view.end()) {
        auto this_arg = *it;

        auto spec_it = arg_specs.end();
        if (this_arg.starts_with("--")) {
            this_arg.remove_prefix(2);
            spec_it = detail::handle_long_arg(this_arg, arg_specs, out);
        } else if (this_arg.starts_with("-")) {
            this_arg.remove_prefix(1);
            spec_it = detail::handle_short_arg(this_arg, arg_specs, out);
        } else {
            detail::handle_bad_arg(this_arg, arg_specs, out);
        }

        // argument name valid, move on to argument values
        ++it;

        const auto& arg_spec = *spec_it;
        using value_vec = std::vector<std::basic_string<Char>>;
        value_vec this_arg_values;

        int n = arg_spec.value_count;
        int remaining_arg_count = static_cast<int>(argv_view.end() - it);
        if (n == -1) {
            n = remaining_arg_count;
        }

        if (n > remaining_arg_count) {
            CPPTOOLS_THROW(exception::arg_parse::not_enough_arguments_supplied_error);
        }

        // n may be 0, in which case no extra parameter shall be consumed
        // and a 0-sized value array will be inserted for that argument
        auto values_end = it + n;
        this_arg_values.reserve(n);
        for (; it != values_end; ++it) {
            this_arg_values.push_back(std::basic_string<Char>(*it));
        }
        result.insert(arg_spec.name, std::move(this_arg_values));
    }

    for (const auto& spec : arg_specs) {
        if (spec.necessity == necessity::required && result.has(spec.name) == false) {
            CPPTOOLS_THROW(exception::arg_parse::required_arg_missing_error);
        }
    }

    return result;
}

using argument_name = basic_argument_name<char>;
using wargument_name = basic_argument_name<wchar_t>;
using argument = basic_argument<char>;
using wargument = basic_argument<wchar_t>;
using value_map = basic_argument_value_map<char>;
using wvalue_map = basic_argument_value_map<wchar_t>;

} // namespace tools::cli

#endif//CPPTOOLS_CLI_ARGUMENT_PARSER_HPP