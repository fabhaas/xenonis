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
                                    mpz_class a;                                                                       \
                                    mpz_ui_pow_ui(a.get_mpz_t(), base_a, exp_a);                                       \
                                    a *= ran_a;                                                                        \
                                    auto mp_a_str{std::string(signed_a ? "-" : "") + a.get_str(16)};                   \
                                    if (signed_a)                                                                      \
                                        a *= -1;                                                                       \
                                                                                                                       \
                                    mpz_class b;                                                                       \
                                    mpz_ui_pow_ui(b.get_mpz_t(), base_b, exp_b);                                       \
                                    b *= ran_b;                                                                        \
                                    auto mp_b_str{std::string(signed_b ? "-" : "") + b.get_str(16)};                   \
                                    if (signed_b)                                                                      \
                                        b *= -1;                                                                       \
                                                                                                                       \
                                    TypeParam b_a(mp_a_str);                                                           \
                                    TypeParam b_b(mp_b_str);                                                           \
                                                                                                                       \
                                    ASSERT_EQ((a op b), (b_a op b_b)) << "b_a: " << b_a << '\n'                        \
                                                                      << "b_b: " << b_b << '\n'                        \
                                                                      << "mp_a: " << mp_a_str << '\n'                  \
                                                                      << "mp_b: " << mp_b_str << '\n';                 \
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
                                    mpz_class a;                                                                       \
                                    mpz_ui_pow_ui(a.get_mpz_t(), base_a, exp_a);                                       \
                                    a *= ran_a;                                                                        \
                                    auto mp_a_str{std::string(signed_a ? "-" : "") + a.get_str(16)};                   \
                                    a *= signed_a ? -1 : 1;                                                            \
                                                                                                                       \
                                    mpz_class b;                                                                       \
                                    mpz_ui_pow_ui(b.get_mpz_t(), base_b, exp_b);                                       \
                                    b *= ran_b;                                                                        \
                                    auto mp_b_str{std::string(signed_b ? "-" : "") + b.get_str(16)};                   \
                                    b *= signed_b ? -1 : 1;                                                            \
                                                                                                                       \
                                    TypeParam b_a(mp_a_str);                                                           \
                                    TypeParam b_b(mp_b_str);                                                           \
                                                                                                                       \
                                    mpz_class c;                                                                       \
                                    c = a op b;                                                                        \
                                    bool mp_c_signed{sgn(c) == -1};                                                    \
                                    if (mp_c_signed)                                                                   \
                                        c *= -1;                                                                       \
                                    auto mp_c_str{std::string(mp_c_signed ? "-" : "") + c.get_str(16)};                \
                                                                                                                       \
                                    auto b_c{b_a op b_b};                                                              \
                                                                                                                       \
                                    ASSERT_EQ(mp_c_str, b_c.to_string()) << "b_a: " << b_a << '\n'                     \
                                                                         << "b_b: " << b_b << '\n'                     \
                                                                         << "mp_a: " << mp_a_str << '\n'               \
                                                                         << "mp_b: " << mp_b_str << '\n';              \
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

#define BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(name_, T)                                                                    \
    TYPED_TEST(util_bigint_test, name_)                                                                                \
    {                                                                                                                  \
        std::random_device ran_device;                                                                                 \
        std::default_random_engine ran_engine(ran_device());                                                           \
        std::uniform_int_distribution<T> ran_dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());       \
                                                                                                                       \
        for (std::size_t i{0}; i < this->ran_count * this->ran_count; ++i) {                                           \
            auto ran = ran_dist(ran_engine);                                                                           \
                                                                                                                       \
            mpz_class a;                                                                                               \
            a = ran;                                                                                                   \
                                                                                                                       \
            TypeParam b_a(ran);                                                                                        \
                                                                                                                       \
            ASSERT_EQ(b_a.to_string(), a.get_str(16)) << "ran:\t\t" << ran << '\n';                                    \
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
                    mpz_class a;
                    mpz_ui_pow_ui(a.get_mpz_t(), base, exp);
                    a *= ran;
                    auto mp_a_str{std::string(sign ? "-" : "") + a.get_str(16)};
                    if (sign)
                        a *= -1;

                    TypeParam b_a(mp_a_str);

                    ASSERT_EQ(b_a.to_string(), a.get_str(16)) << "base:\t\t" << base << '\n'
                                                              << "exponent:\t" << exp << '\n'
                                                              << "random:\t\t" << ran << '\n'
                                                              << "sign:\t\t" << std::boolalpha << sign << '\n';
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

BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_u64, std::uint64_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_u32, std::uint32_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_u16, std::uint16_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_u8, std::uint8_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_i64, std::int64_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_i32, std::int32_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_i16, std::int16_t)
BIGINT_UTIL_CONSTRUCTOR_TEST_CASE(construct_i8, std::int8_t)

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
