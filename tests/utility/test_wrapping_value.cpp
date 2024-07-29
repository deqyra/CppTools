#include <cpptools/utility/wrapping_value.hpp>

#include <catch2/catch_all.hpp>

inline constexpr char TAGS[] = "[utility][wrapping_value]";

namespace tools::test {

TEST_CASE("wrapping_value has correct semantics", TAGS) {
    wrapping_value<signed char, -3, 124> value = 0;
    REQUIRE(value == 0);

    value = 127;
    REQUIRE(value == 0);

    value = -10;
    REQUIRE(value == 117);

    value -= 127;
    REQUIRE(value == 117);
    value += -127;
    REQUIRE(value == 117);
    value -= 2;
    REQUIRE(value == 115);
    value += 8;
    REQUIRE(value == 123);

    REQUIRE(value + 30 == 26);
    REQUIRE(value + 128 == -3);
    REQUIRE(value - 30 == 93);
    REQUIRE(value - 254 == 123);

    wrapping_value<float, -2.5f, 124.5f> fvalue = 0;
    REQUIRE(fvalue == 0);

    fvalue = 127;
    REQUIRE(fvalue == 0.f);

    fvalue = -10;
    REQUIRE(fvalue == 117.f);

    fvalue -= 127;
    REQUIRE(fvalue == 117.f);
    fvalue += -128;
    REQUIRE(fvalue == 116.f);

    fvalue += 2.f;
    REQUIRE(fvalue == 118);
    fvalue -= 43;
    REQUIRE(fvalue == 75.f);

    REQUIRE(fvalue + 30 == 105);
    REQUIRE(fvalue + 49.5f == -2.5f);
    REQUIRE(fvalue - 30 == 45);
    REQUIRE(fvalue - 77.5 == -2.5f);
}

}