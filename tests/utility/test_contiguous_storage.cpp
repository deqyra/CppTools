#include <cpptools/memory/contiguous_storage.hpp>

#include <catch2/catch_all.hpp>

inline constexpr char TAGS[] = "[memory][contiguous_storage]";

namespace tools::test {

TEST_CASE("contiguous_storage provides contiguous storage", TAGS) {
    using storage_t = contiguous_storage<int, float, double[4]>;
    static_assert(storage_t::Size == (sizeof(int) + sizeof(float) + sizeof(double[4])));

    storage_t storage;
    const storage_t& const_storage = storage;

    auto& int_proxy = storage.get<int>();
    int& int_value = int_proxy.value();

    const auto& const_int_proxy = const_storage.get<int>();
    const int& const_int_value = const_int_proxy.value();

    int_value = 3;
    REQUIRE(const_int_value == 3);
    REQUIRE(const_int_proxy.value() == 3);
    int_value = 42;
    REQUIRE(const_int_value == 42);
    REQUIRE(const_int_proxy.value() == 42);
    int_proxy = 69;
    REQUIRE(const_int_value == 69);
    REQUIRE(const_int_proxy.value() == 69);

    REQUIRE(&int_value == reinterpret_cast<int*>(storage.data()));

    auto& float_proxy = storage.get<float>();
    float& float_value = float_proxy.value();

    const auto& const_float_proxy = const_storage.get<float>();
    const float& const_float_value = const_float_proxy.value();

    float_value = 3.f;
    REQUIRE(const_float_value == 3.f);
    REQUIRE(const_float_proxy.value() == 3.f);
    float_value = 42.f;
    REQUIRE(const_float_value == 42.f);
    REQUIRE(const_float_proxy.value() == 42.f);
    float_proxy = 69.f;
    REQUIRE(const_float_value == 69.f);
    REQUIRE(const_float_proxy.value() == 69.f);

    REQUIRE(&float_value == reinterpret_cast<float*>(storage.data() + sizeof(int)));

    auto& double_arr_proxy = storage.get<double[4]>();
    double (&double_arr_value)[4] = double_arr_proxy.value();

    const auto& const_double_arr_proxy = const_storage.get<double[4]>();
    const double (&const_double_arr_value)[4] = const_double_arr_proxy.value();

    double_arr_value[0] = 3.;
    double_arr_value[1] = 4.;
    double_arr_value[2] = 5.;
    double_arr_value[3] = 6.;
    REQUIRE(const_double_arr_value[0] == 3.);
    REQUIRE(const_double_arr_value[1] == 4.);
    REQUIRE(const_double_arr_value[2] == 5.);
    REQUIRE(const_double_arr_value[3] == 6.);
    REQUIRE(const_double_arr_proxy.value()[0] == 3.);
    REQUIRE(const_double_arr_proxy.value()[1] == 4.);
    REQUIRE(const_double_arr_proxy.value()[2] == 5.);
    REQUIRE(const_double_arr_proxy.value()[3] == 6.);
    double_arr_value[0] = 43.;
    double_arr_value[1] = 44.;
    double_arr_value[2] = 45.;
    double_arr_value[3] = 46.;
    REQUIRE(const_double_arr_value[0] == 43.);
    REQUIRE(const_double_arr_value[1] == 44.);
    REQUIRE(const_double_arr_value[2] == 45.);
    REQUIRE(const_double_arr_value[3] == 46.);
    REQUIRE(const_double_arr_proxy.value()[0] == 43.);
    REQUIRE(const_double_arr_proxy.value()[1] == 44.);
    REQUIRE(const_double_arr_proxy.value()[2] == 45.);
    REQUIRE(const_double_arr_proxy.value()[3] == 46.);
    double_arr_proxy[0] = 103.;
    double_arr_proxy[1] = 104.;
    double_arr_proxy[2] = 105.;
    double_arr_proxy[3] = 106.;
    REQUIRE(const_double_arr_value[0] == 103.);
    REQUIRE(const_double_arr_value[1] == 104.);
    REQUIRE(const_double_arr_value[2] == 105.);
    REQUIRE(const_double_arr_value[3] == 106.);
    REQUIRE(const_double_arr_proxy.value()[0] == 103.);
    REQUIRE(const_double_arr_proxy.value()[1] == 104.);
    REQUIRE(const_double_arr_proxy.value()[2] == 105.);
    REQUIRE(const_double_arr_proxy.value()[3] == 106.);

    REQUIRE(&double_arr_value == reinterpret_cast<double(*)[4]>(storage.data() + sizeof(int) + sizeof(float)));
}

}