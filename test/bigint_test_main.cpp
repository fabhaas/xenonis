// ---------- (C) 2018 fahaas ----------
#define CATCH_CONFIG_MAIN
#include "../src/bigint.hpp"
#include <algorithm>
#include <catch2/catch.hpp>
#include <fmt/printf.h>
#include <gmp.h>
#include <iostream>
#include <vector>

#define BIGINT_TEST_CASE(name, op)                                                                                     \
    TEMPLATE_TEST_CASE(#name, "[bigint]", std::uint64_t /*, std::uint32_t, std::uint16_t, std::uint8_t*/)              \
    {                                                                                                                  \
        auto base_a = GENERATE(2, 10, 16);                                                                             \
        auto sign_a = GENERATE(-1, 1);                                                                                 \
        auto exp_a = GENERATE(range(0, 100));                                                                          \
                                                                                                                       \
        auto base_b = GENERATE(2, 10, 16);                                                                             \
        auto sign_b = GENERATE(-1, 1);                                                                                 \
        auto exp_b = GENERATE(range(0, 100));                                                                          \
                                                                                                                       \
        constexpr size_t random_size = 10;                                                                             \
                                                                                                                       \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<uint64_t> ran_dist(0, std::numeric_limits<uint64_t>::max());                     \
                                                                                                                       \
        std::vector<uint64_t> ran(random_size * 2);                                                                    \
        std::for_each(ran.begin(), ran.end(), [&ran_dist, &ran_engine](auto& e) { e = ran_dist(ran_engine); });        \
                                                                                                                       \
        for (size_t i = 0; i < ran.size(); i += 2) {                                                                   \
            mpz_t mp_a;                                                                                                \
            mpz_init(mp_a);                                                                                            \
            mpz_ui_pow_ui(mp_a, static_cast<uint64_t>(base_a), static_cast<uint64_t>(exp_a));                          \
            mpz_mul_si(mp_a, mp_a, sign_a);                                                                            \
            mpz_mul_ui(mp_a, mp_a, ran[i]);                                                                            \
            std::string mp_a_str = mpz_get_str(NULL, 16, mp_a);                                                        \
                                                                                                                       \
            mpz_t mp_b;                                                                                                \
            mpz_init(mp_b);                                                                                            \
            mpz_ui_pow_ui(mp_b, static_cast<uint64_t>(base_b), static_cast<uint64_t>(exp_b));                          \
            mpz_mul_si(mp_b, mp_b, sign_b);                                                                            \
            mpz_mul_ui(mp_b, mp_b, ran[i + 1]);                                                                        \
            std::string mp_b_str = mpz_get_str(NULL, 16, mp_b);                                                        \
                                                                                                                       \
            numeric::bigint<TestType> b_a(mpz_get_str(NULL, 16, mp_a));                                                \
            numeric::bigint<TestType> b_b(mpz_get_str(NULL, 16, mp_b));                                                \
                                                                                                                       \
            mpz_t mp_c;                                                                                                \
            mpz_init(mp_c);                                                                                            \
            mpz_##name(mp_c, mp_a, mp_b);                                                                              \
            std::string mp_c_str = mpz_get_str(NULL, 16, mp_c);                                                        \
                                                                                                                       \
            auto b_c = b_a op b_b;                                                                                     \
                                                                                                                       \
            CAPTURE(b_a, b_b, mp_a_str, mp_b_str, b_c, mp_c_str);                                                      \
            CHECK(mp_c_str == b_c.to_string());                                                                        \
            mpz_clear(mp_a);                                                                                           \
            mpz_clear(mp_b);                                                                                           \
            mpz_clear(mp_c);                                                                                           \
        }                                                                                                              \
    }

BIGINT_TEST_CASE(mul, *)
BIGINT_TEST_CASE(add, +)
