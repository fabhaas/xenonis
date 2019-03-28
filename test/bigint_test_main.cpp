// ---------- (C) 2018 fahaas ----------
#define CATCH_CONFIG_MAIN
#include "../src/bigint.hpp"
#include <algorithm>
#include <catch2/catch.hpp>
#include <gmpxx.h>

constexpr std::array<std::uint64_t, 3> bases = {10, 16};
constexpr std::array<std::uint64_t, 8> exponents = {0, 100, 1000};
constexpr std::size_t ran_count = 100;

TEMPLATE_TEST_CASE("bigint_data", "[bigint_data]", std::uint64_t, std::uint32_t, std::uint16_t)
{
    xenonis::internal::bigint_data<TestType> data;
    constexpr std::uint64_t n = 1024;
    REQUIRE(data.size() == 0);
    REQUIRE(data.capacity() == 0);
    REQUIRE(data.empty());

    data.resize(n);
    REQUIRE(data.size() == n);
    REQUIRE(data.capacity() == n);
    REQUIRE(!data.empty());

    TestType i = 0;
    for (auto first = data.rbegin(); first != data.rend(); ++first, ++i)
        *first = i;

    i = 0;
    for (auto first = data.cbegin(); first != data.cend(); ++first, ++i) {
        CAPTURE(i);
        REQUIRE(*first + i == n - 1);
    }

    data.pop_back();
    REQUIRE(data.size() == n - 1);
    REQUIRE(data.capacity() == n);

    data.pop_n(n - 1);
    REQUIRE(data.empty());
    REQUIRE(data.capacity() == n);
}

TEMPLATE_TEST_CASE("greater and less", "[algorithms]", std::uint64_t, std::uint32_t, std::uint16_t,
                   std::uint8_t)
{
    using container_type = std::vector<TestType>;
    container_type a(1024, 1);
    container_type b(1024, 1);

    SECTION("size not equal")
    {
        a.pop_back();
        REQUIRE(xenonis::algorithms::less<container_type>(a, b));
        REQUIRE(xenonis::algorithms::greater<container_type>(b, a));
    }

    SECTION("size equal")
    {
        a.front() = 2;
        REQUIRE(xenonis::algorithms::less<container_type>(b, a));
        REQUIRE(xenonis::algorithms::greater<container_type>(a, b));
    }

    SECTION("equal")
    {
        REQUIRE(!xenonis::algorithms::less<container_type>(a, b));
        REQUIRE(!xenonis::algorithms::greater<container_type>(a, b));
        REQUIRE(xenonis::algorithms::less<container_type>(a, b, true));
        REQUIRE(xenonis::algorithms::greater<container_type>(a, b, true));
    }
}

TEMPLATE_TEST_CASE("hex_add", "[algorithms]", std::uint64_t, std::uint32_t, std::uint16_t,
                   std::uint8_t)
{
    using container_type = std::vector<TestType>;
    container_type a(2047, std::numeric_limits<TestType>::max());
    container_type b(1024, 2);

    auto c = xenonis::algorithms::hex_add<container_type>(a, b);

    REQUIRE(c[0] == 1);
    for (std::size_t i = 1; i < 1024; ++i) {
        CAPTURE(i);
        REQUIRE(c[i] == 2);
    }

    for (std::size_t i = 1024; i < 2047; ++i) {
        CAPTURE(i);
        REQUIRE(c[i] == 0);
    }
}

TEMPLATE_TEST_CASE("hex_sub", "[algorithms]", std::uint64_t, std::uint32_t, std::uint16_t,
                   std::uint8_t)
{
    using container_type = std::vector<TestType>;
    SECTION("same size and result is 0")
    {
        container_type a(2048);
        container_type b(2048);

        std::iota(a.begin(), a.end(), 0);
        std::iota(b.begin(), b.end(), 0);

        auto c = xenonis::algorithms::hex_sub<container_type>(a, b);
        REQUIRE(c.size() == 1);
        REQUIRE(c.front() == 0);
    }

    SECTION("same size")
    {
        container_type a(2048, 5);
        container_type b(2048, 3);

        auto c = xenonis::algorithms::hex_sub<container_type>(a, b);
        REQUIRE(c.size() == 2048);

        for (std::size_t i = 0; i < 2048; ++i)
            REQUIRE(c[i] == 2);
    }

    SECTION("different size")
    {
        container_type a(2047, 1);
        container_type b(1024, 2);

        auto c = xenonis::algorithms::hex_sub<container_type>(a, b);
        CAPTURE(c);
        REQUIRE(c.size() == 2047);

        REQUIRE(c[0] == std::numeric_limits<TestType>::max());
        for (std::size_t i = 1; i < 1024; ++i) {
            CAPTURE(i);
            REQUIRE(c[i] == std::numeric_limits<TestType>::max() - 1);
        }

        REQUIRE(c[1024] == 0);
        for (std::size_t i = 1025; i < 2047; ++i) {
            CAPTURE(i);
            REQUIRE(c[i] == 1);
        }
    }
}

TEMPLATE_TEST_CASE("to_string", "[bigint]", xenonis::hex_bigint64)
{
    auto base = GENERATE(2, 10, 16);
    auto exp = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);

    constexpr std::size_t random_size = 10;

    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<std::uint64_t> ran_dist(
        0, std::numeric_limits<std::uint64_t>::max());

    std::vector<std::uint64_t> ran(random_size * 10);
    std::for_each(ran.begin(), ran.end(),
                  [&ran_dist, &ran_engine](auto& e) { e = ran_dist(ran_engine); });

    for (std::size_t i = 0; i < ran.size(); i += 2) {
        mpz_class a;
        mpz_ui_pow_ui(a.get_mpz_t(), static_cast<std::uint64_t>(base),
                      static_cast<std::uint64_t>(exp));
        a *= ran[i];
        auto mp_a_str = a.get_str(16);

        TestType b_a(mp_a_str);

        CAPTURE(b_a, mp_a_str);
        CHECK(mp_a_str == b_a.to_string());
    }
}

#define BIGINT_BOOL_OPERATOR_TEST_CASE(name_, op, ...)                                             \
    TEMPLATE_TEST_CASE(#name_, "[bigint]", ##__VA_ARGS__)                                          \
    {                                                                                              \
        constexpr auto name = #name_;                                                              \
        std::random_device ran_device;                                                             \
        std::default_random_engine ran_engine(ran_device());                                       \
        std::uniform_int_distribution<std::uint64_t> ran_dist(                                     \
            0, std::numeric_limits<std::uint64_t>::max());                                         \
        for (const auto& base_a : bases) {                                                         \
            for (const auto& exp_a : exponents) {                                                  \
                for (std::size_t i_a = 0; i_a < ran_count; ++i_a) {                                \
                    auto ran_a = ran_dist(ran_engine);                                             \
                    for (const auto& base_b : bases) {                                             \
                        for (const auto& exp_b : exponents) {                                      \
                            for (std::size_t i_b = 0; i_b < ran_count; ++i_b) {                    \
                                auto ran_b = ran_dist(ran_engine);                                 \
                                auto test = [&](bool signed_a, bool signed_b) {                    \
                                    mpz_class a;                                                   \
                                    mpz_ui_pow_ui(a.get_mpz_t(), base_a, exp_a);                   \
                                    a *= ran_a;                                                    \
                                    auto mp_a_str =                                                \
                                        std::string(signed_a ? "-" : "") + a.get_str(16);          \
                                    if (signed_a)                                                  \
                                        a *= -1;                                                   \
                                                                                                   \
                                    mpz_class b;                                                   \
                                    mpz_ui_pow_ui(b.get_mpz_t(), base_b, exp_b);                   \
                                    b *= ran_b;                                                    \
                                    auto mp_b_str =                                                \
                                        std::string(signed_b ? "-" : "") + b.get_str(16);          \
                                    if (signed_b)                                                  \
                                        b *= -1;                                                   \
                                                                                                   \
                                    TestType b_a(mp_a_str);                                        \
                                    TestType b_b(mp_b_str);                                        \
                                                                                                   \
                                    CAPTURE(name, b_a, b_b, a.get_str(16), b.get_str(16));         \
                                    CHECK((a op b) == (b_a op b_b));                               \
                                };                                                                 \
                                test(false, false);                                                \
                                test(true, false);                                                 \
                                test(false, true);                                                 \
                                test(true, true);                                                  \
                            }                                                                      \
                        }                                                                          \
                    }                                                                              \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
    }

#define BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(name_, op, ...)                                       \
    TEMPLATE_TEST_CASE(#name_, "[bigint]", ##__VA_ARGS__)                                          \
    {                                                                                              \
        constexpr auto name = #name_;                                                              \
        std::random_device ran_device;                                                             \
        std::default_random_engine ran_engine(ran_device());                                       \
        std::uniform_int_distribution<std::uint64_t> ran_dist(                                     \
            0, std::numeric_limits<std::uint64_t>::max());                                         \
        for (const auto& base_a : bases) {                                                         \
            for (const auto& exp_a : exponents) {                                                  \
                for (std::size_t i_a = 0; i_a < ran_count; ++i_a) {                                \
                    auto ran_a = ran_dist(ran_engine);                                             \
                    for (const auto& base_b : bases) {                                             \
                        for (const auto& exp_b : exponents) {                                      \
                            for (std::size_t i_b = 0; i_b < ran_count; ++i_b) {                    \
                                auto ran_b = ran_dist(ran_engine);                                 \
                                auto test = [&](bool signed_a, bool signed_b) {                    \
                                    mpz_class a;                                                   \
                                    mpz_ui_pow_ui(a.get_mpz_t(), base_a, exp_a);                   \
                                    a *= ran_a;                                                    \
                                    auto mp_a_str =                                                \
                                        std::string(signed_a ? "-" : "") + a.get_str(16);          \
                                    a *= signed_a ? -1 : 1;                                        \
                                                                                                   \
                                    mpz_class b;                                                   \
                                    mpz_ui_pow_ui(b.get_mpz_t(), base_b, exp_b);                   \
                                    b *= ran_b;                                                    \
                                    auto mp_b_str =                                                \
                                        std::string(signed_b ? "-" : "") + b.get_str(16);          \
                                    b *= signed_b ? -1 : 1;                                        \
                                                                                                   \
                                    TestType b_a(mp_a_str);                                        \
                                    TestType b_b(mp_b_str);                                        \
                                                                                                   \
                                    mpz_class c;                                                   \
                                    c = a op b;                                                    \
                                    bool mp_c_signed = sgn(c) == -1;                               \
                                    if (mp_c_signed)                                               \
                                        c *= -1;                                                   \
                                    auto mp_c_str =                                                \
                                        std::string(mp_c_signed ? "-" : "") + c.get_str(16);       \
                                                                                                   \
                                    auto b_c = b_a op b_b;                                         \
                                                                                                   \
                                    CAPTURE(name, b_a, b_b, b_c, mp_a_str, mp_b_str, mp_c_str);    \
                                    CHECK(mp_c_str == b_c.to_string());                            \
                                };                                                                 \
                                test(false, false);                                                \
                                test(true, false);                                                 \
                                test(false, true);                                                 \
                                test(true, true);                                                  \
                            }                                                                      \
                        }                                                                          \
                    }                                                                              \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
    }

#define BIGINT_COMPLETE_TEST(...)                                                                  \
    BIGINT_BOOL_OPERATOR_TEST_CASE(less, <, ##__VA_ARGS__)                                         \
    BIGINT_BOOL_OPERATOR_TEST_CASE(greater, >, ##__VA_ARGS__)                                      \
    BIGINT_BOOL_OPERATOR_TEST_CASE(less_equal, <=, ##__VA_ARGS__)                                  \
    BIGINT_BOOL_OPERATOR_TEST_CASE(greater_equal, >=, ##__VA_ARGS__)                               \
    BIGINT_BOOL_OPERATOR_TEST_CASE(equal, ==, ##__VA_ARGS__)                                       \
                                                                                                   \
    BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(add, +, ##__VA_ARGS__)                                    \
    BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(sub, -, ##__VA_ARGS__)                                    \
    BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(mul, *, ##__VA_ARGS__)

BIGINT_COMPLETE_TEST(xenonis::hex_bigint)
