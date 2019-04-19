// ---------- (C) 2018, 2019 fahaas ----------
/*!
 *  \file arithmetic.hpp
 *  Implements the arithmetic algorithms used in bigint.hpp
 */
#pragma once

#include "../integer_traits.hpp"
#include "compare.hpp"
#include "util.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <type_traits>

namespace xenonis::algorithms {
    /*!
     *  Adds b to a and writes the result to c. Requires a.size() >= b.size() and c.size() >= a.size(). It is possible that a is c (then b will be added to a).
     *  \param a_first iterator poiting to first element of a. Could be const iterator. Could be equal c.
     *  \param b_first iterator pointing to first element of b. Could be const iterator. Must not be equal c.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \param c_first iterator pointing to first element of c. Could be equal a.
     *  \returns carry
     */
    template <class InIter, class OutIter>
    bool hex_add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

    /*!
     *  Subtracts b from a and writes the result to c. Requires a.size() >= b.size() and c.size() >= a.size(). c must not be a.
     *  \param a_first iterator poiting to first element of a. Could be const iterator.
     *  \param b_first iterator pointing to first element of b. Could be const iterator.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \param c_first iterator pointing to first element of c.
     *  \returns carry
     */
    template <class InIter, class OutIter>
    bool hex_sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

    /*!
     *  Subtracts b from a and writes the result to a. Requires a.size() >= b.size().
     *  \param a_first iterator poiting to first element of a.
     *  \param b_first iterator pointing to first element of b. Could be const iterator.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \returns carry
     */
    template <class InIter, class InOutIter> bool hex_sub_from(InOutIter a_first, InIter b_first, InIter b_last);

    /*!
     *  Increments a by 1.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a
     *  \returns carry
     */
    template <class InIter> bool hex_increment(InIter a_first, InIter a_last);

    /*!
     *  Decrements a by 1.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \returns carry
     */
    template <class InIter> bool hex_decrement(InIter a_first, InIter a_last);

    /*template <class OutContainer, class InIter>
    OutContainer hex_mul_with_digit(const InIter a_first, const InIter a_last,
                                    std::remove_reference_t<decltype(*a_first)> digit);*/

    /*!
     *  Multiplies a with b and returns the result.
     *  \details Uses the naive method to multiply. Complexity: O(n^2)
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \returns the result
     */
    template <class OutContainer, class InIter>
    OutContainer hex_naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last);

    /*!
     *  Multiplies a with b and returns the result.
     *  \details Uses the Karatsuba Algorithm to multiply which is a recursive algorithm with a complexity of O(n^log2(3)). See e.g https://en.wikipedia.org/wiki/Karatsuba_algorithm.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \returns the result
     */
    template <class OutContainer, class InIter>
    OutContainer hex_karatsuba_mul(const InIter a_first, const InIter a_last, const InIter& b_first,
                                   const InIter& b_last);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer hex_div(const InContainer& a, const InContainer& b);

    template <class InIter, class OutIter> bool hex_add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first + *b_first + carry;
            carry = carry ? *c_first <= *b_first : *c_first < *b_first;
        }
        return carry;
    }

    template <class InIter, class OutIter> bool hex_sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first - *b_first - carry;
            carry = carry ? *c_first >= *a_first : *c_first > *a_first;
        }
        return carry;
    }

    template <class InIter, class InOutIter> bool hex_sub_from(InOutIter a_first, InIter b_first, InIter b_last)
    {
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first) {
            auto tmp = *a_first;
            *a_first -= *b_first + carry;
            carry = carry ? *a_first >= tmp : *a_first > tmp;
        }
        return carry;
    }

    template <class InIter> bool hex_increment(InIter a_first, InIter a_last)
    {
        for (; a_first != a_last; ++a_first)
            if (++(*a_first) != 0)
                return false;
        return true;
    }

    template <class InIter> bool hex_decrement(InIter a_first, InIter a_last)
    {
        for (; a_first != a_last; ++a_first)
            if (--(*a_first) != std::numeric_limits<std::remove_reference_t<decltype(*a_first)>>::max())
                return false;
        return true;
    }

    /*template <class OutContainer, class InIter>
    OutContainer hex_mul_with_digit(const InIter a_first, const InIter a_last,
                                    std::remove_reference_t<decltype(*a_first)> digit)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
        using doubled = typename traits::uint<value_type>::doubled;

        auto a_size = std::distance(a_first, a_last);

        OutContainer ret(a_size + 1, 0);

        std::unique_ptr<doubled[]> tmp(new doubled[a_size]);
        std::fill(tmp.get(), tmp.get() + a_size, 0);

        for (std::size_t i = 0; i < a_size; ++i)
            tmp[i] = static_cast<doubled>(a_first[i]) * digit;

        auto* tmp_ptr = reinterpret_cast<value_type*>(tmp.get());
        ret[0] = tmp_ptr[0];

        bool carry = false;
        for (std::size_t i_ret = 1, i_tmp = 2; i_ret < ret.size() - 1; ++i_ret, i_tmp += 2) {
            ret[i_ret] = tmp_ptr[i_tmp] + tmp_ptr[i_tmp - 1] + carry;
            carry = carry ? ret[i_ret] <= tmp_ptr[i_tmp] : ret[i_ret] < tmp_ptr[i_tmp];
        }

        ret[ret.size() - 1] = tmp_ptr[a_size * 2 - 1];

        if (carry)
            ++(ret[ret.size() - 1]);

        if (ret.back() == 0)
            ret.pop_back();

        return ret;
    }

    template <class OutContainer, class InIter>
    OutContainer hex_naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        auto a_size = std::distance(a_first, a_last);
        auto b_size = std::distance(b_first, b_last);

        OutContainer ret(a_size + b_size, 0);

        for (decltype(a_size) i = 0; i < b_size; ++i) {
            auto tmp = hex_mul_with_digit<OutContainer>(a_first, a_last, b_first[i]);
            if (hex_add(ret.cbegin() + i, tmp.cbegin(), tmp.cend(), ret.begin() + i))
                hex_increment(ret.begin() + i + tmp.size(), ret.end());
        }

        remove_zeros(ret);
        return ret;
    }*/

    template <class OutContainer, class InIter>
    OutContainer hex_naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
        using doubled = typename traits::uint<value_type>::doubled;

        auto a_size = std::distance(a_first, a_last);
        auto b_size = std::distance(b_first, b_last);

        OutContainer ret(a_size + b_size, 0);

        const auto in_last = a_last;
        const auto out_last = ret.end();

        doubled n0 = 0;
        doubled n1 = 0;
        auto* n0_ptr = reinterpret_cast<value_type*>(&n0);
        auto* n1_ptr = reinterpret_cast<value_type*>(&n1);

        for (auto ret_first = ret.begin(); b_first != b_last; ++b_first, ++ret_first) {
            const auto digit = *b_first;
            if (digit == 0)
                continue;
            auto in_first = a_first;
            auto out_first = ret_first;

            value_type carry = 0;

            n0 = static_cast<doubled>(*(in_first++)) * digit;
            *out_first += n0_ptr[0];
            carry = *(out_first++) < n0_ptr[0];

            while (in_first != in_last - (a_size % 2 == 0)) {
                n1 = static_cast<doubled>(*(in_first++)) * digit;
                *out_first += n0_ptr[1] + carry;
                carry = carry ? *out_first <= n0_ptr[1] : *out_first < n0_ptr[1];
                *out_first += n1_ptr[0];
                carry += carry ? *(out_first++) <= n1_ptr[0] : *(out_first++) < n1_ptr[0];

                n0 = static_cast<doubled>(*(in_first++)) * digit;
                *out_first += n1_ptr[1] + carry;
                carry = carry ? *out_first <= n1_ptr[1] : *out_first < n1_ptr[1];
                *out_first += n0_ptr[0];
                carry += carry ? *(out_first++) <= n0_ptr[0] : *(out_first++) < n0_ptr[0];
            }

            if (a_size % 2 == 0) {
                n1 = static_cast<doubled>(*(in_first++)) * digit;
                *out_first += n0_ptr[1] + carry;
                carry = carry ? *out_first <= n0_ptr[1] : *out_first < n0_ptr[1];
                *out_first += n1_ptr[0];
                carry += carry ? *(out_first++) <= n1_ptr[0] : *(out_first++) < n1_ptr[0];

                *out_first += n1_ptr[1] + carry;

                if (carry) {
                    if (*(out_first++) <= n1_ptr[1])
                        hex_increment(out_first, out_last);
                } else {
                    if (*(out_first++) < n1_ptr[1])
                        hex_increment(out_first, out_last);
                }
            } else {
                *out_first += n0_ptr[1] + carry;

                if (carry) {
                    if (*(out_first++) <= n0_ptr[1])
                        hex_increment(out_first, out_last);
                } else {
                    if (*(out_first++) < n0_ptr[1])
                        hex_increment(out_first, out_last);
                }
            }
        }

        remove_zeros(ret);
        return ret;
    }

    template <class OutContainer, class InIter>
    OutContainer hex_karatsuba_mul(const InIter a_first, const InIter a_last, const InIter& b_first,
                                   const InIter& b_last)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
        using doubled = typename traits::uint<value_type>::doubled;

        auto a_size = std::distance(a_first, a_last);
        auto b_size = std::distance(b_first, b_last);

        if (a_size == 1 || b_size == 1) {
            if (a_size == b_size) {
                OutContainer tmp(2);
                doubled ret = static_cast<doubled>(*a_first) * (*b_first);
                auto* ret_ptr = reinterpret_cast<value_type*>(&ret);
                tmp[0] = ret_ptr[0];
                tmp[1] = ret_ptr[1];
                return tmp;
            } else if (a_size < b_size) {
                return hex_naive_mul<OutContainer>(b_first, b_last, a_first, a_last);
            } else {
                return hex_naive_mul<OutContainer>(a_first, a_last, b_first, b_last);
            }
        }

        auto max_size = std::max(a_size, b_size);
        if (max_size % 2 == 1)
            ++max_size;
        const auto limb_size = max_size / 2;

        InIter a_l_first = a_first;
        InIter a_l_last;
        InIter b_l_first = b_first;
        InIter b_l_last;
        InIter a_h_first;
        InIter a_h_last;
        InIter b_h_first;
        InIter b_h_last;

        bool a_h_zero;
        bool b_h_zero;

        std::size_t a_h_size;
        std::size_t b_h_size;

        if (limb_size > a_size) {
            a_l_last = a_last;
            a_h_zero = true;
            a_h_size = 0;
            a_h_first = nullptr;
            a_h_last = nullptr;
        } else {
            a_l_last = a_first + limb_size; //maybe - 1
            a_h_first = a_first + limb_size;
            a_h_last = a_last;
            a_h_zero = is_zero(a_h_first, a_h_last);
            a_h_size = std::distance(a_h_first, a_h_last);
        }

        if (limb_size > b_size) {
            b_l_last = b_last;
            b_h_zero = true;
            b_h_size = 0;
            b_h_first = nullptr;
            b_h_last = nullptr;
        } else {
            b_l_last = b_first + limb_size; //maybe - 1
            b_h_first = b_first + limb_size;
            b_h_last = b_last;
            b_h_zero = is_zero(b_h_first, b_h_last);
            b_h_size = std::distance(b_h_first, b_h_last);
        }

        bool a_l_zero = is_zero(a_first, a_last);
        bool b_l_zero = is_zero(b_l_first, b_l_last);

        if (a_h_zero && a_l_zero)
            return OutContainer(1, 0);

        if (b_h_zero && b_l_zero)
            return OutContainer(1, 0);

        if (a_h_zero) { // a_l_zero is false
            if (b_h_zero)
                return hex_karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last);

            if (b_l_zero) // b_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_h_first, b_h_last), limb_size);

            auto x = hex_karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last);
            auto y = hex_karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last);

            OutContainer ret(a_size + b_size, 0);

            std::copy(x.begin(), x.end(), ret.begin() + limb_size);

            if (algorithms::hex_add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::hex_increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (b_h_zero) { // b_l_zero is false
            if (a_h_zero)
                return hex_karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_l_first, a_l_last);

            if (a_l_zero) // a_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_h_first, a_h_last), limb_size);

            auto x = hex_karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last);
            auto y = hex_karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_l_first, a_l_last);

            OutContainer ret(a_size + b_size, 0);

            std::copy(x.begin(), x.end(), ret.begin() + limb_size);

            if (algorithms::hex_add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::hex_increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (a_l_zero) { // a_h_zero is false
            if (b_l_zero)
                return lshift<OutContainer, OutContainer>(
                    hex_karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last), max_size);

            auto x = hex_karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last);
            auto y = lshift<OutContainer, OutContainer>(
                hex_karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last), limb_size);

            OutContainer ret(a_size + b_size, 0);
            std::copy(x.begin(), x.end(), ret.begin() + max_size);

            if (algorithms::hex_add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::hex_increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (b_l_zero) { // b_h_zero is false
            auto x = hex_karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_h_first, a_h_last);
            auto y = lshift<OutContainer, OutContainer>(
                hex_karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last), limb_size);

            OutContainer ret(a_size + b_size, 0);
            std::copy(x.begin(), x.end(), ret.begin() + max_size);

            if (algorithms::hex_add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::hex_increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        OutContainer p1;
        OutContainer p2;
        OutContainer p3;

        //calculate p1 and p2
        p1 = hex_karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last);
        p2 = hex_karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last);

        auto add = [](auto a_first, auto a_last, auto b_first, auto b_last, auto a_size, auto b_size) {
            auto add = [](auto a_first, auto a_last, auto b_first, auto b_last, auto a_size, auto b_size) {
                OutContainer tmp(a_size + 1);
                tmp.back() = 0;
                if (xenonis::algorithms::hex_add(a_first, b_first, b_last, tmp.begin())) {
                    std::copy(a_first + b_size, a_last, tmp.begin() + b_size);
                    xenonis::algorithms::hex_increment(tmp.begin() + b_size, tmp.end());
                } else {
                    std::copy(a_first + b_size, a_last, tmp.begin() + b_size);
                    tmp.pop_back();
                }
                return std::move(tmp);
            };

            if (a_size >= b_size)
                return add(a_first, a_last, b_first, b_last, a_size, b_size);
            else
                return add(b_first, b_last, a_first, a_last, b_size, a_size);
        };

        //calculate p3
        auto p3_1 = add(a_l_first, a_l_last, a_h_first, a_h_last, limb_size, std::distance(a_h_first, a_h_last));
        auto p3_2 = add(b_l_first, b_l_last, b_h_first, b_h_last, limb_size, std::distance(b_h_first, b_h_last));

        p3 = hex_karatsuba_mul<OutContainer>(p3_1.begin(), p3_1.end(), p3_2.begin(), p3_2.end());

        if (hex_sub_from(p3.begin(), p1.begin(), p1.end()))
            hex_decrement(p3.begin() + p1.size(), p3.end());

        if (hex_sub_from(p3.begin(), p2.begin(), p2.end()))
            hex_decrement(p3.begin() + p2.size(), p3.end());

        remove_zeros(p3);

        //calculate result
        OutContainer ret(a_size + b_size, 0);
        std::copy(p1.begin(), p1.end(), ret.begin() + max_size);

        if (hex_add(ret.cbegin(), p2.cbegin(), p2.cend(), ret.begin()))
            hex_increment(ret.begin() + p2.size(), ret.end());

        if (hex_add(ret.cbegin() + limb_size, p3.cbegin(), p3.cend(), ret.begin() + limb_size))
            hex_increment(ret.begin() + p3.size() + limb_size, ret.end());

        remove_zeros(ret);
        return ret;
    }
} // namespace xenonis::algorithms
