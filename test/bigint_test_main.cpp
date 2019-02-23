// ---------- (C) 2018 fahaas ----------
#define CATCH_CONFIG_MAIN
#include "../src/bigint.hpp"
#include <algorithm>
#include <catch2/catch.hpp>
#include <fmt/printf.h>
#include <gmp.h>
#include <iostream>
#include <vector>

#define BIGINT_OPERATOR_TEST_CASE(name, op)                                                        \
    TEMPLATE_TEST_CASE(#name, "[bigint]", xenonis::hex_bigint64)                                   \
    {                                                                                              \
        auto base_a = GENERATE(2, 10, 16);                                                         \
        auto sign_a = GENERATE(1, 1);                                                              \
        auto exp_a = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);                                 \
                                                                                                   \
        auto base_b = GENERATE(2, 10, 16);                                                         \
        auto sign_b = GENERATE(1, 1);                                                              \
        auto exp_b = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);                                 \
                                                                                                   \
        constexpr std::size_t random_size = 10000;                                                  \
                                                                                                   \
        std::random_device ran_device;                                                             \
        std::default_random_engine ran_engine(ran_device());                                       \
        std::uniform_int_distribution<std::uint64_t> ran_dist(                                     \
            0, std::numeric_limits<std::uint64_t>::max());                                         \
                                                                                                   \
        std::vector<std::uint64_t> ran(random_size * 2);                                           \
        std::for_each(ran.begin(), ran.end(),                                                      \
                      [&ran_dist, &ran_engine](auto& e) { e = ran_dist(ran_engine); });            \
                                                                                                   \
        for (std::size_t i = 0; i < ran.size(); i += 2) {                                          \
            mpz_t mp_a;                                                                            \
            mpz_init(mp_a);                                                                        \
            mpz_ui_pow_ui(mp_a, static_cast<std::uint64_t>(base_a),                                \
                          static_cast<std::uint64_t>(exp_a));                                      \
            mpz_mul_si(mp_a, mp_a, sign_a);                                                        \
            mpz_mul_ui(mp_a, mp_a, ran[i]);                                                        \
            std::string mp_a_str = mpz_get_str(NULL, 16, mp_a);                                    \
                                                                                                   \
            mpz_t mp_b;                                                                            \
            mpz_init(mp_b);                                                                        \
            mpz_ui_pow_ui(mp_b, static_cast<std::uint64_t>(base_b),                                \
                          static_cast<std::uint64_t>(exp_b));                                      \
            mpz_mul_si(mp_b, mp_b, sign_b);                                                        \
            mpz_mul_ui(mp_b, mp_b, ran[i + 1]);                                                    \
            std::string mp_b_str = mpz_get_str(NULL, 16, mp_b);                                    \
                                                                                                   \
            TestType b_a(mp_a_str);                                                                \
            TestType b_b(mp_b_str);                                                                \
                                                                                                   \
            mpz_t mp_c;                                                                            \
            mpz_init(mp_c);                                                                        \
            mpz_##name(mp_c, mp_a, mp_b);                                                          \
            std::string mp_c_str = mpz_get_str(NULL, 16, mp_c);                                    \
                                                                                                   \
            auto b_c = b_a op b_b;                                                                 \
                                                                                                   \
            CAPTURE(b_a, b_b, b_c, mp_a_str, mp_b_str, mp_c_str);                                  \
            CHECK(mp_c_str == b_c.to_string());                                                    \
            mpz_clear(mp_a);                                                                       \
            mpz_clear(mp_b);                                                                       \
            mpz_clear(mp_c);                                                                       \
        }                                                                                          \
    }

TEMPLATE_TEST_CASE("sub", "[bigint]", xenonis::hex_bigint64)
{
    auto base_a = GENERATE(2, 10, 16);
    auto sign_a = GENERATE(1, 1);
    auto exp_a = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);

    auto base_b = GENERATE(2, 10, 16);
    auto sign_b = GENERATE(1, 1);
    auto exp_b = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);

    constexpr std::size_t random_size = 10000;

    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<std::uint64_t> ran_dist(
        0, std::numeric_limits<std::uint64_t>::max());

    std::vector<std::uint64_t> ran(random_size * 2);
    std::for_each(ran.begin(), ran.end(),
                  [&ran_dist, &ran_engine](auto& e) { e = ran_dist(ran_engine); });

    for (std::size_t i = 0; i < ran.size(); i += 2) {
        mpz_t mp_a;
        mpz_init(mp_a);
        mpz_ui_pow_ui(mp_a, static_cast<std::uint64_t>(base_a), static_cast<std::uint64_t>(exp_a));
        mpz_mul_si(mp_a, mp_a, sign_a);
        mpz_mul_ui(mp_a, mp_a, ran[i]);
        std::string mp_a_str = mpz_get_str(NULL, 16, mp_a);

        mpz_t mp_b;
        mpz_init(mp_b);
        mpz_ui_pow_ui(mp_b, static_cast<std::uint64_t>(base_b), static_cast<std::uint64_t>(exp_b));
        mpz_mul_si(mp_b, mp_b, sign_b);
        mpz_mul_ui(mp_b, mp_b, ran[i + 1]);
        std::string mp_b_str = mpz_get_str(NULL, 16, mp_b);

        TestType b_a(mp_a_str);
        TestType b_b(mp_b_str);

        mpz_t mp_c;
        mpz_init(mp_c);
        mpz_sub(mp_c, mp_a, mp_b);
        bool mp_c_signed = mpz_sgn(mp_c) == -1;
        if (mp_c_signed)
            mpz_mul_si(mp_c, mp_c, -1);
        std::string mp_c_str = std::string(mp_c_signed ? "-" : "") + mpz_get_str(NULL, 16, mp_c);

        auto b_c = b_a - b_b;

        CAPTURE(b_a, b_b, b_c, mp_a_str, mp_b_str, mp_c_str);
        CHECK(mp_c_str == b_c.to_string());
        mpz_clear(mp_a);
        mpz_clear(mp_b);
        mpz_clear(mp_c);
    }
}

TEMPLATE_TEST_CASE("to_string", "[bigint]", xenonis::hex_bigint64)
{
    auto base = GENERATE(2, 10, 16);
    auto exp = GENERATE(0, 10, 25, 50, 100, 250, 500, 1000);

    constexpr std::size_t random_size = 1000;

    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<std::uint64_t> ran_dist(
        0, std::numeric_limits<std::uint64_t>::max());

    std::vector<std::uint64_t> ran(random_size * 10);
    std::for_each(ran.begin(), ran.end(),
                  [&ran_dist, &ran_engine](auto& e) { e = ran_dist(ran_engine); });

    for (std::size_t i = 0; i < ran.size(); i += 2) {
        mpz_t mp_a;
        mpz_init(mp_a);
        mpz_ui_pow_ui(mp_a, static_cast<std::uint64_t>(base), static_cast<std::uint64_t>(exp));
        mpz_mul_ui(mp_a, mp_a, ran[i]);
        std::string mp_a_str = mpz_get_str(NULL, 16, mp_a);

        TestType b_a(mp_a_str);

        CAPTURE(b_a, mp_a_str);
        CHECK(mp_a_str == b_a.to_string());
        mpz_clear(mp_a);
    }
}

// BIGINT_OPERATOR_TEST_CASE(mul, *)
BIGINT_OPERATOR_TEST_CASE(add, +)
// BIGINT_OPERATOR_TEST_CASE(sub, -)
