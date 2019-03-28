// ---------- (C) 2018 fahaas ----------
/*!
 *	\file algorithms.hpp
 *	\brief Declaration and definition of algorithms used in xenonis::bigint
 */
#pragma once

#include "bigint_config.hpp"

#include "bigint_data.hpp"
#include "integer_traits.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace xenonis::algorithms {
    // declarations
    template <typename Value, class InContainer>
    std::string hex_to_string(const InContainer& a, bool is_signed, bool lower_case = true);

    template <typename Value, class OutContainer>
    OutContainer hex_from_string(const std::string_view str);

    /*!
     * \brief Appends count zeros to the front. e.g data = { 1, 2, 3 }, count = 3 -> { 0, 0, 0, 1,
     * 2, 3 }
     */
    template <class InContainer, class OutContainer = InContainer>
    OutContainer lshift(const InContainer& data, decltype(data.size()) count);

    /*!
     * \brief Removes all redundant zeros. z.B { 0, 1, 2 } -> { 1, 2 }
     * \details Never returns empty container. z.B { 0, 0, 0 } -> { 0 }
     */
    template <class InContainer> void remove_zeros(InContainer& data) noexcept;

    template <class InContainer> InContainer remove_zeros(InContainer&& data) noexcept;

    template <class InContainer>
    bool greater(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer>
    bool less(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer> bool is_zero(const InContainer& a) noexcept;

    template <class InContainer> void hex_add_to(InContainer& a, const InContainer& b);

    template <class InContainer>
    void hex_add_to_lshifted(InContainer& a, const InContainer& b, std::size_t count);

    template <class InContainer, class OutContainer = InContainer>
    OutContainer hex_add(const InContainer& a, const InContainer& b);

    template <class InContainer, class OutContainer = InContainer>
    OutContainer hex_add_lshifted(const InContainer& a, const InContainer& b, std::size_t count);

    template <class InContainer, class OutContainer = InContainer>
    OutContainer hex_sub(const InContainer& a, const InContainer& b);

    template <typename Value, class InContainer, class OutContainer>
    OutContainer hex_mul_with_digit(const InContainer& a, Value digit);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer hex_basecase_mul(const InContainer& a, const InContainer& b);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer hex_karatsuba_mul(const InContainer& a, const InContainer& b);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer hex_div(const InContainer& a, const InContainer& b);

    // definitions
    template <typename Value, class InContainer>
    std::string hex_to_string(const InContainer& data, bool is_signed, bool lower_case)
    {
        using size_type = decltype(data.size());
        std::string ret(data.size() * sizeof(Value) * 2, '\0');

        // convert to hex
        constexpr std::uint8_t mask0 = 0xF0;
        constexpr std::uint8_t mask1 = 0xF;

        size_type i_ret = 0;
        std::for_each(data.cbegin(), data.cend(), [&ret, &i_ret](Value n) {
            auto* n_ptr = reinterpret_cast<std::uint8_t*>(&n);

            for (size_type i = 0; i < sizeof(Value); ++i) {
                ret[i_ret++] = static_cast<char>(mask1 & n_ptr[i]);
                ret[i_ret++] = static_cast<char>((mask0 & n_ptr[i]) >> 4);
            }
        });

        // remove zeros
        remove_zeros(ret);

        // convert to string
        const char letter_dis = lower_case ? 87 : 55;
        std::for_each(ret.begin(), ret.end(), [&letter_dis](char& c) {
            if (c < 10)
                c += 48;
            else
                c += letter_dis;
        });

        // add sign
        if (is_signed)
            ret += '-';

        // reverse
        std::reverse(ret.begin(), ret.end());

        return ret;
    }

    template <typename Value, class OutContainer>
    OutContainer hex_from_string(const std::string_view str)
    {
        auto conv = [](auto& c) {
            if (c >= 48 && c <= 57)
                return c - 48;
            else if (c >= 65 && c <= 90)
                return c - 55;
            else if (c >= 97 && c <= 122)
                return c - 87;
            else
                throw std::invalid_argument("Input string not valid!: invalid char");
        };

        constexpr std::uint8_t block_size = sizeof(Value) * 2;

        OutContainer ret(str.size() / block_size + (str.size() % block_size != 0), 0);

        auto ret_first = ret.begin();
        std::uint8_t i_block = 0;
        auto str_first = str.crbegin();
        auto str_last = str.crend();
        for (; str_first != str_last; ++str_first) {
            *ret_first |= (static_cast<Value>(conv(*str_first)) << (4 * (i_block++)));

            if (i_block == block_size) {
                i_block = 0;
                ++ret_first;
            }
        }

        return ret;
    }

    template <class InContainer, class OutContainer>
    OutContainer lshift(const InContainer& data, decltype(data.size()) count)
    {
        OutContainer tmp(data.size() + count, 0);
        std::copy(data.cbegin(), data.cend(), tmp.begin() + count);
        return tmp;
    }

    template <class InContainer> void remove_zeros(InContainer& data) noexcept
    {
        auto first = data.crbegin();
        auto last = data.crend();

        for (; first != last; ++first)
            if (*first != 0)
                break;

        if (first == last) { // data.size() must not be 0
            data.resize(1);
            data.front() = 0;
            return;
        }

        data.resize(last - first);
    }

    template <class InContainer> InContainer remove_zeros(InContainer&& data) noexcept
    {
        auto first = data.crbegin();
        auto last = data.crend();

        for (; first != last; ++first)
            if (*first != 0)
                break;

        if (first == last) { // data.size() must not be 0
            data.resize(1);
            data.front() = 0;
            return std::move(data);
        }

        data.resize(last - first);
        return std::move(data);
    }

    template <class InContainer>
    bool greater(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() > b.size();

        for (std::size_t i = a.size() - 1; i < a.size(); --i) {
            if (a[i] != b[i])
                return a[i] > b[i];
        }
        return or_equal;
    }

    template <class InContainer>
    bool less(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() < b.size();

        for (std::size_t i = a.size() - 1; i < a.size(); --i) {
            if (a[i] != b[i])
                return a[i] < b[i];
        }

        return or_equal; // a == b
    }

    template <class InContainer> bool is_zero(const InContainer& a) noexcept
    {
        for (auto e : a)
            if (e != 0)
                return false;

        return true;
    }

    template <class InContainer> void hex_add_to(InContainer& a, const InContainer& b)
    {
        assert(a.size() >= b.size());

        auto a_first = a.begin();
        auto a_last = a.end();
        auto b_first = b.cbegin();
        auto b_last = b.cend();

        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first) {
            *a_first = *a_first + *b_first + carry;
            carry = carry ? *a_first <= *b_first : *a_first < *b_first;
        }

        if (carry) {
            for (; a_first != a_last; ++a_first) {
                ++(*a_first);
                if (*a_first != 0) {
                    carry = false;
                    break;
                }
            }

            assert(!carry);
        }
    }

    template <class InContainer>
    void hex_add_to_lshifted(InContainer& a, const InContainer& b, std::size_t count)
    {
        assert(a.size() >= b.size());

        auto a_first = a.begin() + count;
        auto a_last = a.end();
        auto b_first = b.cbegin();
        auto b_last = b.cend();

        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first) {
            *a_first = *a_first + *b_first + carry;
            carry = carry ? *a_first <= *b_first : *a_first < *b_first;
        }

        if (carry) {
            for (; a_first != a_last; ++a_first) {
                ++(*a_first);
                if (*a_first != 0) {
                    carry = false;
                    break;
                }
            }

            assert(!carry);
        }
    }

    template <class InContainer, class OutContainer>
    OutContainer hex_add(const InContainer& a, const InContainer& b)
    {
        assert(a.size() >= b.size());

        OutContainer c(a.size() + 1);

        auto a_first = a.cbegin();
        auto a_last = a.cend();
        auto b_first = b.cbegin();
        auto b_last = b.cend();
        auto c_first = c.begin();

        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first + *b_first + carry;
            carry = carry ? *c_first <= *a_first : *c_first < *a_first;
        }

        if (carry) {
            for (; a_first != a_last; ++a_first, ++c_first) {
                *c_first = *a_first + 1;
                if (*c_first > *a_first) {
                    std::copy(++a_first, a_last, ++c_first);
                    carry = false;
                    break;
                }
            }

            if (!carry)
                c.pop_back();
            else
                c.back() = 1;
        } else {
            std::copy(a_first, a_last, c_first);
            c.pop_back();
        }

        return c;
    }

    template <class InContainer, class OutContainer /*= InContainer*/>
    OutContainer hex_add_lshifted(const InContainer& a, const InContainer& b, std::size_t count)
    {
        assert(a.size() >= b.size());

        OutContainer c(a.size() + 1 + count);

        auto a_first = a.cbegin();
        auto a_last = a.cend();
        auto b_first = b.cbegin();
        auto b_last = b.cend();
        auto c_first = c.begin() + count;

        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first + *b_first + carry;
            carry = carry ? *c_first <= *a_first : *c_first < *a_first;
        }

        if (carry) {
            for (; a_first != a_last; ++a_first, ++c_first) {
                *c_first = *a_first + 1;
                if (*c_first > *a_first) {
                    std::copy(++a_first, a_last, ++c_first);
                    carry = false;
                    break;
                }
            }

            if (!carry)
                c.pop_back();
            else
                c.back() = 1;
        } else {
            std::copy(a_first, a_last, c_first);
            c.pop_back();
        }

        return c;
    }

    template <class InContainer, class OutContainer>
    OutContainer hex_sub(const InContainer& a, const InContainer& b)
    {
        assert(a.size() >= b.size());

        OutContainer c(a.size());

        auto a_first = a.cbegin();
        auto a_last = a.cend();
        auto b_first = b.cbegin();
        auto b_last = b.cend();
        auto c_first = c.begin();

        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first - *b_first - carry;
            carry = carry ? *c_first >= *a_first : *c_first > *a_first;
        }

        if (carry) {
            for (; a_first != a_last; ++a_first, ++c_first) {
                *c_first = *a_first - 1;
                if (*c_first < *a_first) {
                    std::copy(++a_first, a_last, ++c_first);
                    carry = false;
                    break;
                }
            }

            assert(!carry);
        } else {
            std::copy(a_first, a_last, c_first);
        }

        algorithms::remove_zeros(c);
        return c;
    }

    template <typename Value, class InContainer, class OutContainer>
    OutContainer hex_mul_with_digit(const InContainer& a, Value digit)
    {
        using doubled = typename traits::uint<Value>::doubled;
        using size_type = decltype(a.size());

        OutContainer ret(a.size() + 1, 0);

        std::unique_ptr<doubled[]> tmp(new doubled[a.size()]);
        std::fill(tmp.get(), tmp.get() + a.size(), 0);

        for (size_type i = 0; i < a.size(); ++i)
            tmp[i] = static_cast<doubled>(a[i]) * digit;

        auto* tmp_ptr = reinterpret_cast<Value*>(tmp.get());
        ret[0] = tmp_ptr[0];

        bool carry = false;
        for (size_type i_ret = 1, i_tmp = 2; i_ret < ret.size() - 1; ++i_ret, i_tmp += 2) {
            ret[i_ret] = tmp_ptr[i_tmp] + tmp_ptr[i_tmp - 1] + carry;
            carry = carry ? ret[i_ret] <= tmp_ptr[i_tmp] || ret[i_ret] <= tmp_ptr[i_tmp - 1]
                          : ret[i_ret] < tmp_ptr[i_tmp] || ret[i_ret] < tmp_ptr[i_tmp - 1];
        }

        ret[ret.size() - 1] = tmp_ptr[a.size() * 2 - 1];

        if (carry)
            ++(ret[ret.size() - 1]);

        if (ret.back() == 0)
            ret.pop_back();

        return ret;
    }

    template <typename Value, class InContainer, class OutContainer>
    OutContainer hex_basecase_mul(const InContainer& a, const InContainer& b)
    {
        OutContainer ret(a.size() + b.size(), 0);

        for (decltype(a.size()) i = 0; i < b.size(); ++i)
            hex_add_to_lshifted(ret, hex_mul_with_digit<Value, OutContainer, OutContainer>(a, b[i]),
                                i);
        remove_zeros(ret);
        return ret;
    }

    // TODO: parallelize
    template <typename Value, class InContainer, class OutContainer>
    OutContainer hex_karatsuba_mul(const InContainer& a, const InContainer& b)
    {
        auto add = [](const auto& a, const auto& b) {
            if (a.size() >= b.size())
                return hex_add<OutContainer, OutContainer>(a, b);
            else
                return hex_add<OutContainer, OutContainer>(b, a);
        };

        if (a.size() == 1 || b.size() == 1) {
            if (a.size() == b.size()) {
                OutContainer tmp(2);
                typename traits::uint<Value>::doubled ret =
                    static_cast<typename traits::uint<Value>::doubled>(a.front()) * b.front();
                auto* ret_ptr = reinterpret_cast<Value*>(&ret);
                tmp[0] = ret_ptr[0];
                tmp[1] = ret_ptr[1];
                return tmp;
            } else if (a.size() < b.size()) {
                return hex_mul_with_digit<Value, InContainer, OutContainer>(b, a.front());
            } else {
                return hex_mul_with_digit<Value, InContainer, OutContainer>(a, b.front());
            }
        }

        auto max_size = std::max(a.size(), b.size());
        if (max_size % 2 == 1)
            ++max_size;
        const auto limb_size = max_size / 2;

        auto high = [&limb_size](const InContainer& data) -> std::optional<OutContainer> {
            if (limb_size < data.size()) {
                OutContainer ret(data.size() - limb_size);
                std::copy(data.cbegin() + limb_size, data.cend(), ret.begin());
                return ret;
            } else {
                return std::nullopt;
            }
        };

        auto low = [&limb_size](const InContainer& data) {
            if (limb_size > data.size()) {
                OutContainer ret = data;
                return ret;
            }

            OutContainer ret(limb_size);
            std::copy(data.cbegin(), data.cbegin() + limb_size, ret.begin());
            return ret;
        };

        auto a_l = low(a);
        auto b_l = low(b);
        auto opt_a_h = high(a);
        auto opt_b_h = high(b);

        bool a_h_zero = opt_a_h.has_value() ? is_zero(opt_a_h.value()) : true;
        bool a_l_zero = is_zero(a_l);
        bool b_h_zero = opt_b_h.has_value() ? is_zero(opt_b_h.value()) : true;
        bool b_l_zero = is_zero(b_l);

        if (a_h_zero && a_l_zero)
            return OutContainer(1, 0);

        if (b_h_zero && b_l_zero)
            return OutContainer(1, 0);

        if (a_h_zero) { // a_l_zero is false
            if (b_h_zero)
                return hex_karatsuba_mul<Value, OutContainer, OutContainer>(a_l, b_l);

            if (b_l_zero) // b_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<Value, OutContainer, OutContainer>(a_l, *opt_b_h),
                    max_size / 2);

            return add(lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_b_h, a_l),
                           max_size / 2),
                       hex_karatsuba_mul<Value, OutContainer, OutContainer>(a_l, b_l));
        }

        if (b_h_zero) { // b_l_zero is false
            if (a_h_zero)
                return hex_karatsuba_mul<Value, OutContainer, OutContainer>(b_l, a_l);

            if (a_l_zero) // a_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<Value, OutContainer, OutContainer>(b_l, *opt_a_h),
                    max_size / 2);

            return add(lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_a_h, b_l),
                           max_size / 2),
                       hex_karatsuba_mul<Value, OutContainer, OutContainer>(b_l, a_l));
        }

        if (a_l_zero) { // a_h_zero is false
            if (b_l_zero)
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_a_h, *opt_b_h),
                    max_size);

            return add(lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_a_h, *opt_b_h),
                           max_size),
                       lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_a_h, b_l),
                           max_size / 2));
        }

        if (b_l_zero) // b_h_zero is false
            return add(lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_b_h, *opt_a_h),
                           max_size),
                       lshift<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_b_h, a_l),
                           max_size / 2));

        auto p1 = hex_karatsuba_mul<Value, OutContainer, OutContainer>(*opt_a_h, *opt_b_h);
        auto p2 = hex_karatsuba_mul<Value, OutContainer, OutContainer>(a_l, b_l);
        return add(hex_add<OutContainer, OutContainer>(
                       lshift<OutContainer, OutContainer>(p1, max_size), p2),
                   lshift<OutContainer, OutContainer>(
                       hex_sub<OutContainer, OutContainer>(
                           hex_karatsuba_mul<Value, OutContainer, OutContainer>(
                               hex_add<OutContainer, OutContainer>(a_l, *opt_a_h),
                               hex_add<OutContainer, OutContainer>(b_l, *opt_b_h)),
                           add(p1, p2)),
                       max_size / 2));
    }
} // namespace xenonis::algorithms
