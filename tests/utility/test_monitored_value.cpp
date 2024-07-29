#include <cpptools/utility/monitored_value.hpp>

#include <catch2/catch_all.hpp>

inline constexpr char TAGS[] = "[utility][monitored_value]";

namespace {

struct foo {
    static inline int copy_count = 0;
    static inline int move_count = 0;

    foo() = delete;
    foo(int)        {}
    foo(const foo&) { ++copy_count; }
    foo(foo&&)      { ++move_count; }
    foo& operator=(const foo&) { ++copy_count; return *this; }
    foo& operator=(foo&&)      { ++move_count; return *this; }
};

}

namespace tools::test {

TEST_CASE("monitored_value has correct semantics", TAGS) {
    bool modified_int = false;
    monitored_value<int> value(modified_int);

    REQUIRE(value == 0);

    value = 127;
    REQUIRE(value == 127);
    REQUIRE(modified_int);
    modified_int = false;

    value += 1;
    REQUIRE(value == 128);
    REQUIRE(modified_int);
    modified_int = false;

    value -= 1;
    REQUIRE(value == 127);
    REQUIRE(modified_int);
    modified_int = false;

    value *= 2;
    REQUIRE(value == 254);
    REQUIRE(modified_int);
    modified_int = false;

    value /= 2;
    REQUIRE(value == 127);
    REQUIRE(modified_int);
    modified_int = false;

    value %= 3;
    REQUIRE(value == 1);
    REQUIRE(modified_int);
    modified_int = false;

    bool modified_foo = false;
    monitored_value f(modified_foo, foo(0));
    auto f1 = foo(1);
    int original_copy_count = foo::copy_count;
    int original_move_count = foo::move_count;

    f = f1;
    REQUIRE(modified_foo);
    REQUIRE((foo::copy_count - original_copy_count) == 1);
    REQUIRE_FALSE(modified_int);
    modified_foo = false;

    f = foo(2);
    REQUIRE(modified_foo);
    REQUIRE((foo::move_count - original_move_count) == 1);
    REQUIRE_FALSE(modified_int);
    modified_foo = false;
}

}