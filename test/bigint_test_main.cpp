//******************************************************************************
//* Copyright 2018-2020 Fabian Haas                                            *
//*                                                                            *
//* This Source Code Form is subject to the terms of the Mozilla Public        *
//* License, v. 2.0. If a copy of the MPL was not distributed with this        *
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.                  *
//******************************************************************************

#include <algorithm>
#include <bigint.hpp>
#include <gmpxx.h>
#include <gtest/gtest.h>
#include <random>

#define BIGINT_BOOL_OPERATOR_TEST_CASE(name_, op)                                                                      \
    TYPED_TEST(bool_bigint_test, name_)                                                                                \
    {                                                                                                                  \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<std::uint64_t> ran_dist(0, std::numeric_limits<std::uint64_t>::max());           \
        for (const auto& base_a : this->bases) {                                                                       \
            for (const auto& exp_a : this->exponents) {                                                                \
                for (std::size_t i_a{0}; i_a < this->ran_count; ++i_a) {                                               \
                    auto ran_a{ran_dist(ran_engine)};                                                                  \
                    for (const auto& base_b : this->bases) {                                                           \
                        for (const auto& exp_b : this->exponents) {                                                    \
                            for (std::size_t i_b{0}; i_b < this->ran_count; ++i_b) {                                   \
                                auto ran_b{ran_dist(ran_engine)};                                                      \
                                auto test = [&](bool signed_a, bool signed_b) {                                        \
                                    mpz_t a;                                                                           \
                                    mpz_init(a);                                                                       \
                                    mpz_ui_pow_ui(a, base_a, exp_a);                                                   \
                                    mpz_mul_ui(a, a, ran_a);                                                           \
                                    std::unique_ptr<char> mp_a_tmp{mpz_get_str(NULL, 16, a)};                          \
                                    auto mp_a_str{std::string(signed_a ? "-" : "") + mp_a_tmp.get()};                  \
                                    if (signed_a)                                                                      \
                                        mpz_neg(a, a);                                                                 \
                                                                                                                       \
                                    mpz_t b;                                                                           \
                                    mpz_init(b);                                                                       \
                                    mpz_ui_pow_ui(b, base_b, exp_b);                                                   \
                                    mpz_mul_ui(b, b, ran_b);                                                           \
                                    std::unique_ptr<char> mp_b_tmp{mpz_get_str(NULL, 16, b)};                          \
                                    auto mp_b_str{std::string(signed_b ? "-" : "") + mp_b_tmp.get()};                  \
                                    if (signed_b)                                                                      \
                                        mpz_neg(b, b);                                                                 \
                                                                                                                       \
                                    TypeParam b_a(mp_a_str);                                                           \
                                    TypeParam b_b(mp_b_str);                                                           \
                                                                                                                       \
                                    ASSERT_EQ((mpz_cmp(a, b) op 0), (b_a op b_b)) << "b_a: " << b_a << '\n'            \
                                                                                  << "b_b: " << b_b << '\n'            \
                                                                                  << "mp_a: " << mp_a_str << '\n'      \
                                                                                  << "mp_b: " << mp_b_str << '\n';     \
                                    mpz_clear(a);                                                                      \
                                    mpz_clear(b);                                                                      \
                                };                                                                                     \
                                test(false, false);                                                                    \
                                test(true, false);                                                                     \
                                test(false, true);                                                                     \
                                test(true, true);                                                                      \
                            }                                                                                          \
                        }                                                                                              \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    }

#define BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(name_, op)                                                                \
    TYPED_TEST(arithmetic_bigint_test, name_)                                                                          \
    {                                                                                                                  \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<std::uint64_t> ran_dist(0, std::numeric_limits<std::uint64_t>::max());           \
        for (const auto& base_a : this->bases) {                                                                       \
            for (const auto& exp_a : this->exponents) {                                                                \
                for (std::size_t i_a{0}; i_a < this->ran_count; ++i_a) {                                               \
                    auto ran_a{ran_dist(ran_engine)};                                                                  \
                    for (const auto& base_b : this->bases) {                                                           \
                        for (const auto& exp_b : this->exponents) {                                                    \
                            for (std::size_t i_b{0}; i_b < this->ran_count; ++i_b) {                                   \
                                auto ran_b{ran_dist(ran_engine)};                                                      \
                                auto test = [&](bool signed_a, bool signed_b) {                                        \
                                    mpz_t a;                                                                           \
                                    mpz_init(a);                                                                       \
                                    mpz_ui_pow_ui(a, base_a, exp_a);                                                   \
                                    mpz_mul_ui(a, a, ran_a);                                                           \
                                    std::unique_ptr<char> mp_a_tmp{mpz_get_str(NULL, 16, a)};                          \
                                    auto mp_a_str{std::string(signed_a ? "-" : "") + mp_a_tmp.get()};                  \
                                    if (signed_a)                                                                      \
                                        mpz_neg(a, a);                                                                 \
                                                                                                                       \
                                    mpz_t b;                                                                           \
                                    mpz_init(b);                                                                       \
                                    mpz_ui_pow_ui(b, base_b, exp_b);                                                   \
                                    mpz_mul_ui(b, b, ran_b);                                                           \
                                    std::unique_ptr<char> mp_b_tmp{mpz_get_str(NULL, 16, b)};                          \
                                    auto mp_b_str{std::string(signed_b ? "-" : "") + mp_b_tmp.get()};                  \
                                    if (signed_b)                                                                      \
                                        mpz_neg(b, b);                                                                 \
                                                                                                                       \
                                    TypeParam b_a(mp_a_str);                                                           \
                                    TypeParam b_b(mp_b_str);                                                           \
                                                                                                                       \
                                    mpz_t c;                                                                           \
                                    mpz_init(c);                                                                       \
                                    mpz_##name_(c, a, b);                                                              \
                                    bool mp_c_signed{mpz_sgn(c) == -1};                                                \
                                    if (mp_c_signed)                                                                   \
                                        mpz_neg(c, c);                                                                 \
                                    std::unique_ptr<char> mp_c_tmp{mpz_get_str(NULL, 16, c)};                          \
                                    auto mp_c_str{std::string(mp_c_signed ? "-" : "") + mp_c_tmp.get()};               \
                                                                                                                       \
                                    auto b_c{b_a op b_b};                                                              \
                                                                                                                       \
                                    ASSERT_EQ(mp_c_str, b_c.to_string()) << "b_a: " << b_a << '\n'                     \
                                                                         << "b_b: " << b_b << '\n'                     \
                                                                         << "mp_a: " << mp_a_str << '\n'               \
                                                                         << "mp_b: " << mp_b_str << '\n';              \
                                    mpz_clear(a);                                                                      \
                                    mpz_clear(b);                                                                      \
                                    mpz_clear(c);                                                                      \
                                };                                                                                     \
                                test(false, false);                                                                    \
                                test(true, false);                                                                     \
                                test(false, true);                                                                     \
                                test(true, true);                                                                      \
                            }                                                                                          \
                        }                                                                                              \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    }

#define BIGINT_UTIL_UI_CONSTRUCTOR_TEST_CASE(name_, T)                                                                 \
    TYPED_TEST(util_bigint_test, name_)                                                                                \
    {                                                                                                                  \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<T> ran_dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());       \
                                                                                                                       \
        for (std::size_t i{0}; i < this->ran_count * this->ran_count; ++i) {                                           \
            auto ran = ran_dist(ran_engine);                                                                           \
                                                                                                                       \
            mpz_t a;                                                                                                   \
            mpz_init(a);                                                                                               \
            mpz_set_ui(a, ran);                                                                                        \
                                                                                                                       \
            std::unique_ptr<char> mp_a_str{mpz_get_str(NULL, 16, a)};                                                  \
                                                                                                                       \
            TypeParam b_a(ran);                                                                                        \
                                                                                                                       \
            ASSERT_EQ(b_a.to_string(), mp_a_str.get()) << "ran:\t\t" << ran << '\n';                                   \
                                                                                                                       \
            mpz_clear(a);                                                                                              \
        }                                                                                                              \
    }

#define BIGINT_UTIL_SI_CONSTRUCTOR_TEST_CASE(name_, T)                                                                 \
    TYPED_TEST(util_bigint_test, name_)                                                                                \
    {                                                                                                                  \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<T> ran_dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());       \
                                                                                                                       \
        for (std::size_t i{0}; i < this->ran_count * this->ran_count; ++i) {                                           \
            auto ran = ran_dist(ran_engine);                                                                           \
                                                                                                                       \
            mpz_t a;                                                                                                   \
            mpz_init(a);                                                                                               \
            mpz_set_si(a, ran);                                                                                        \
                                                                                                                       \
            std::unique_ptr<char> mp_a_str{mpz_get_str(NULL, 16, a)};                                                  \
                                                                                                                       \
            TypeParam b_a(ran);                                                                                        \
                                                                                                                       \
            ASSERT_EQ(b_a.to_string(), mp_a_str.get()) << "ran:\t\t" << ran << '\n';                                   \
                                                                                                                       \
            mpz_clear(a);                                                                                              \
        }                                                                                                              \
    }

template <class bigint_type> class arithmetic_bigint_test : public ::testing::Test {
  public:
    const std::array<std::uint64_t, 3> bases{{10, 16}};
    const std::array<std::uint64_t, 8> exponents{{0, 100, 1000}};
    const std::size_t ran_count{100};
};

template <class bigint_type> class bool_bigint_test : public ::testing::Test {
  public:
    const std::array<std::uint64_t, 3> bases{{10, 16}};
    const std::array<std::uint64_t, 8> exponents{{0, 100, 1000}};
    const std::size_t ran_count{10};
};

template <class bigint_type> class util_bigint_test : public ::testing::Test {
  public:
    const std::array<std::uint64_t, 3> bases{{10, 16}};
    const std::array<std::uint64_t, 8> exponents{{0, 100, 1000}};
    const std::size_t ran_count{1000};
};

using bigint_test_types = ::testing::Types<xenonis::bigint64>;
TYPED_TEST_CASE(arithmetic_bigint_test, bigint_test_types);
TYPED_TEST_CASE(bool_bigint_test, bigint_test_types);
TYPED_TEST_CASE(util_bigint_test, bigint_test_types);

TYPED_TEST(util_bigint_test, to_string)
{
    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<std::uint64_t> ran_dist(0, std::numeric_limits<std::uint64_t>::max());
    for (const auto& base : this->bases) {
        for (const auto& exp : this->exponents) {
            for (std::size_t i{0}; i < this->ran_count; ++i) {
                auto ran{ran_dist(ran_engine)};
                auto test = [&](bool sign) {
                    mpz_t a;
                    mpz_init(a);
                    mpz_ui_pow_ui(a, base, exp);
                    mpz_mul_ui(a, a, ran);

                    auto mp_a_str{std::string(base == 0 ? "" : (sign ? "-" : "")) +
                                  std::unique_ptr<char>{mpz_get_str(NULL, 16, a)}.get()};
                    if (sign)
                        mpz_neg(a, a);

                    TypeParam b_a(mp_a_str);

                    ASSERT_EQ(b_a.to_string(), mp_a_str) << "base:\t\t" << base << '\n'
                                                         << "exponent:\t" << exp << '\n'
                                                         << "random:\t\t" << ran << '\n'
                                                         << "sign:\t\t" << std::boolalpha << sign << '\n';
                    mpz_clear(a);
                };
                test(false);
                test(true);
            }
        }
    }
}

BIGINT_BOOL_OPERATOR_TEST_CASE(less, <)
BIGINT_BOOL_OPERATOR_TEST_CASE(greater, >)
BIGINT_BOOL_OPERATOR_TEST_CASE(less_equal, <=)
BIGINT_BOOL_OPERATOR_TEST_CASE(greater_equal, >=)
BIGINT_BOOL_OPERATOR_TEST_CASE(equal, ==)

BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(add, +)
BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(sub, -)
BIGINT_ARITHMETIC_OPERATOR_TEST_CASE(mul, *)

BIGINT_UTIL_UI_CONSTRUCTOR_TEST_CASE(construct_u64, std::uint64_t)
BIGINT_UTIL_UI_CONSTRUCTOR_TEST_CASE(construct_u32, std::uint32_t)
BIGINT_UTIL_UI_CONSTRUCTOR_TEST_CASE(construct_u16, std::uint16_t)
BIGINT_UTIL_UI_CONSTRUCTOR_TEST_CASE(construct_u8, std::uint8_t)
BIGINT_UTIL_SI_CONSTRUCTOR_TEST_CASE(construct_i64, std::int64_t)
BIGINT_UTIL_SI_CONSTRUCTOR_TEST_CASE(construct_i32, std::int32_t)
BIGINT_UTIL_SI_CONSTRUCTOR_TEST_CASE(construct_i16, std::int16_t)
BIGINT_UTIL_SI_CONSTRUCTOR_TEST_CASE(construct_i8, std::int8_t)

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
