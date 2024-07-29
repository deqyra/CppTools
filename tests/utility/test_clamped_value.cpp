#include <cpptools/utility/clamped_value.hpp>

#include <catch2/catch_all.hpp>

inline constexpr char TAGS[] = "[utility][clamped_value]";

namespace tools::test {

TEST_CASE("clamped_value has correct semantics", TAGS) {
    clamped_value<signed char, -3, 124> value = 0;
    REQUIRE(value == 0);

    value = 127;
    REQUIRE(value == 124);

    value = -10;
    REQUIRE(value == -3);

    value -= 127;
    REQUIRE(value == -3);
    value += -128;
    REQUIRE(value == -3);

    value += 2;
    REQUIRE(value == -1);
    value -= -43;
    REQUIRE(value == 42);

    value += 127;
    REQUIRE(value == 124);
    value -= -128;
    REQUIRE(value == 124);

    value -= 30;
    REQUIRE(value == 94);
    value += -30;
    REQUIRE(value == 64);

    REQUIRE(value + 30 == 94);
    REQUIRE(value + 128 == 124);
    REQUIRE(value - 30 == 34);
    REQUIRE(value - 256 == -3);

    clamped_value<float, -2.5f, 124.5f> fvalue = 0;
    REQUIRE(fvalue == 0);

    fvalue = 127;
    REQUIRE(fvalue == 124.5f);

    fvalue = -10;
    REQUIRE(fvalue == -2.5f);

    fvalue -= 127;
    REQUIRE(fvalue == -2.5f);
    fvalue += -128;
    REQUIRE(fvalue == -2.5f);

    fvalue += 2;
    REQUIRE(fvalue == -0.5);
    fvalue -= -43;
    REQUIRE(fvalue == 42.5f);

    fvalue += 127;
    REQUIRE(fvalue == 124.5f);
    fvalue -= -128;
    REQUIRE(fvalue == 124.5f);

    fvalue -= 30;
    REQUIRE(fvalue == 94.5);
    fvalue += -30;
    REQUIRE(fvalue == 64.5);

    REQUIRE(fvalue + 30 == 94.5);
    REQUIRE(fvalue + 127 == 124.5f);
    REQUIRE(fvalue - 30 == 34.5);
    REQUIRE(fvalue - 254 == -2.5f);
}

}