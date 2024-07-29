#include <cpptools/utility/bitwise_enum_ops.hpp>

#include <catch2/catch_all.hpp>

inline constexpr char TAGS[] = "[utility][bitwise_enum_ops]";

namespace tools {

enum class enabled : unsigned int {
    a = 0x00000001, b = 0x00000002, c = 0x00000004, d = 0x00000008,
    e = 0x00000010, f = 0x00000020, g = 0x00000040, h = 0x00000080,
};

enum class disabled {
    a = 0x00000001, b = 0x00000002, c = 0x00000004, d = 0x00000008,
    e = 0x00000010, f = 0x00000020, g = 0x00000040, h = 0x00000080,
};

template<>
static constexpr bool enable_bitwise_enum_v<enabled> = true;

bool fallback_operator_was_called = false;

disabled operator|(disabled left, disabled right) {
    fallback_operator_was_called = true;

    return {};
};

TEST_CASE("bitwise enum ops work for enabled enums", TAGS) {
    using namespace bitwise_enum_ops;

    enabled a = enabled::a;
    enabled b = enabled::b;
    enabled c = enabled::c;
    REQUIRE( (a |= (enabled::b | enabled::e)) == enabled{0x00000013} );
    REQUIRE( (b &= (enabled::b & enabled::b)) == enabled::b          );
    REQUIRE( (c ^= (enabled::c ^ enabled::d)) == enabled::d          );
    REQUIRE( ~enabled::c == enabled{0xfffffffb} );
    REQUIRE( any(a) );
    REQUIRE( none(enabled{}) );
    REQUIRE_FALSE( any(enabled{}) );
    REQUIRE_FALSE( none(a) );
}

TEST_CASE("bitwise enum ops don't work for disabled enums", TAGS) {
    using namespace bitwise_enum_ops;

    REQUIRE( (disabled::a | disabled::e) == disabled{} );
    REQUIRE( fallback_operator_was_called );
    fallback_operator_was_called = false;
}

}