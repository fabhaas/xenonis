// ---------- (C) 2018 fahaas ----------
/*!
 *	\file algorithms.hpp
 *	\brief definition and implementation of algorithms used in numeric::bigint
 */
#pragma once

#include "bigint_config.hpp"

#include "integer_traits.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <type_traits>
#include <vector>

namespace numeric::algorithms {
    using default_size_type = std::size_t;
    template <typename data_type> using default_container_type = std::vector<data_type>;

    template <typename data_type, class container_type, typename size_type>
    container_type rshift(const container_type& data, size_type count);

    template <typename T> void remove_zeros(T& container)
    {
        auto first = container.rbegin();
        auto last = container.rend();

        for (; first != last; ++first)
            if (*first != 0)
                break;

        container.resize(last - first);
    }

    // not wokring implementation of Karatsuba Algorithm
    template <typename data_type, class container_class = default_container_type<data_type>>
    container_class mult(container_class a, container_class c)
    {
        assert(a.size() == c.size());

        if (a.size() == 2) {
            using d_data_type = typename traits::get_doubled<data_type>::type;

            d_data_type ret00 = static_cast<d_data_type>(a[0]) * static_cast<d_data_type>(c[0]);
            d_data_type ret01 = static_cast<d_data_type>(a[0]) * static_cast<d_data_type>(c[1]);
            d_data_type ret10 = static_cast<d_data_type>(a[1]) * static_cast<d_data_type>(c[0]);
            d_data_type ret11 = static_cast<d_data_type>(a[1]) * static_cast<d_data_type>(c[1]);

            auto* ret00_ptr = reinterpret_cast<data_type*>(&ret00);
            auto* ret01_ptr = reinterpret_cast<data_type*>(&ret01);
            auto* ret10_ptr = reinterpret_cast<data_type*>(&ret10);
            auto* ret11_ptr = reinterpret_cast<data_type*>(&ret11);

            container_class ret(4);
            ret[0] = ret00_ptr[0] + ret01_ptr[0];
            if (ret[0] < ret00_ptr[0] || ret[0] < ret01_ptr[0])
                ret[1] = 1;

            ret[1] = ret00_ptr[1] + ret01_ptr[1];
            if (ret[1] < ret00_ptr[1] || ret[1] < ret01_ptr[1])
                ret[2] = 1;

            ret[2] = ret10_ptr[0] + ret11_ptr[0];
            if (ret[2] < ret10_ptr[0] || ret[2] < ret11_ptr[0])
                ret[3] = 1;

            ret[3] = ret10_ptr[1] + ret11_ptr[1];
            if (ret[3] < ret10_ptr[1] || ret[3] < ret11_ptr[1])
                ret.push_back(1);

            return ret;
        }

        const auto shift_count = a.size();

        // requires a.size() == b.size() && a > b
        auto sub = [](container_class a, container_class b) {
            container_class c(a.size());

            auto a_first = a.begin();
            auto a_last = a.end();
            auto b_first = b.begin();
            auto c_first = c.begin();

            bool carry = false;
            for (; a_first != a_last; ++a_first) {
                *c_first = *a_first - *b_first;
                if (carry) {
                    --(*c_first);
                    if (!(*c_first >= *a_first || *c_first >= *b_first))
                        carry = false;
                    continue;
                }
                if (*c_first > *a_first || *c_first > *b_first)
                    carry = true;
            }

            return std::move(c);
        };

        auto add = [](container_class a, container_class b) {
            a.push_back(0);
            b.push_back(0);

            container_class c(a.size());

            auto a_first = a.begin();
            auto a_last = a.end();
            auto b_first = b.begin();
            auto c_first = c.begin();

            bool carry = false;
            for (; a_first != a_last; ++a_first, ++b_first, ++c_first) {
                *c_first = *a_first + *b_first;
                if (carry) {
                    ++(*c_first);
                    if (!(*c_first <= *a_first || *c_first <= *b_first))
                        carry = false;
                    continue;
                }
                if (*c_first < *a_first || *c_first < *b_first)
                    carry = true;
            }

            return std::move(c);
        };

        assert(a.size() % 2 == 0 && c.size() % 2 == 0);

        container_class b(std::make_move_iterator(a.begin() + a.size() / 2), std::make_move_iterator(a.end()));
        a.erase(a.begin() + a.size() / 2, a.end());

        container_class d(std::make_move_iterator(c.begin() + c.size() / 2), std::make_move_iterator(c.end()));
        c.erase(c.begin() + c.size() / 2, c.end());

        assert(a.size() == b.size() && c.size() == d.size() && a.size() == c.size());

        container_class ac = mult<data_type>(a, c);
        container_class bd = mult<data_type>(b, d);
        container_class ab_cd = mult<data_type>(add(a, b), add(c, d));

        return add(add(rshift<data_type>(ac, shift_count), rshift<data_type>(sub(ab_cd, add(ac, bd)), shift_count / 2)),
                   bd);
    }

    template <typename data_type, class container_type = default_container_type<data_type>,
              typename d_data_type = typename traits::get_doubled<data_type>::type,
              class d_container_type = default_container_type<d_data_type>, typename size_type = std::size_t>
    container_type mult_with_digit(const container_type& data, data_type digit)
    {
        d_container_type tmp(data.size(), static_cast<d_data_type>(0));

        for (size_type i_data = 0; i_data < data.size(); ++i_data)
            tmp[i_data] = static_cast<d_data_type>(data[i_data]) * digit;

        auto* tmp_p = reinterpret_cast<data_type*>(tmp.data());

        container_type result(tmp.size() + 1, 0);

        result[0] = tmp_p[0];

        bool carry = false;
        for (size_type i_res = 1, i_tmp = 2; i_res < result.size() - 1; ++i_res, i_tmp += 2) {
            result[i_res] = tmp_p[i_tmp] + tmp_p[i_tmp - 1];
            if (carry) {
                ++(result[i_res]);
                if (!(result[i_res] <= tmp_p[i_tmp] || result[i_res] <= tmp_p[i_tmp - 1]))
                    carry = false;
                continue;
            }
            if (result[i_res] < tmp_p[i_tmp] || result[i_res] < tmp_p[i_tmp - 1])
                carry = true;
        }

        result[result.size() - 1] = tmp_p[tmp.size() * 2 - 1];
        if (carry) {
            ++(result[result.size() - 1]);
            if (result.back() < tmp_p[tmp.size() * 2 - 1]) {
                result.push_back(1);
            }
        }

        if (result.back() == 0)
            result.pop_back();

        return result;
    }

    template <typename data_type, class container_type, typename size_type>
    container_type rshift(const container_type& data, size_type count)
    {
        container_type tmp(data.size() + count, 0);
        std::copy(data.begin(), data.end(), tmp.begin() + count);
        return tmp;
    }

    // requires that a.size() == b.size() == c.size()
    // a and b should have a redundant zero at a.back() or b.back()
    template <typename data_type,
              class container_type = default_container_type<data_type> /*, typename size_type = default_size_type*/>
    void add(const container_type& a, const container_type& b, container_type& c)
    {
        /* the same but without iterators
        bool carry = false;
        auto data_size = data.size();
        for (size_type i = 0; i < data_size; ++i) {
            result[i] = data[i] + other.data[i];
            if (carry) {
                ++(result[i]);
                if (!(result[i] <= data[i] || result[i] <= other.data[i]))
                    carry = false;
                continue;
            }
            if (result[i] < data[i] || result[i] < other.data[i])
                carry = true;
        }*/

        auto a_first = a.begin();
        auto a_last = a.end();
        auto b_first = b.begin();
        auto out_first = c.begin();

        bool carry = false;
        for (; a_first != a_last; ++a_first, ++b_first, ++out_first) {
            *out_first = *a_first + *b_first;
            if (carry) {
                ++(*out_first);
                if (!(*out_first <= *a_first || *out_first <= *b_first))
                    carry = false;
                continue;
            }
            if (*out_first < *a_first || *out_first < *b_first)
                carry = true;
        }
    }

    // requires sorted input
    template <typename data_type, class container_type = default_container_type<data_type>,
              typename size_type = default_size_type>
    void sum_vec(const std::vector<container_type>& vec, container_type& result)
    {
        result.resize(vec.back().size());
        std::copy(vec.back().begin(), vec.back().end(), result.begin());

        for (size_type i = vec.size() - 2; i < vec.size(); --i) {
            bool carry = false;
            for (size_type j = 0; j < vec[i].size(); ++j) {
                result[j] += vec[i][j];
                if (carry) {
                    ++(result[j]);
                    if (result[j] > vec[i][j])
                        carry = false;
                    continue;
                }
                if (result[j] < vec[i][j])
                    carry = true;
            }

            if (carry) {
                for (size_type j = vec[i].size(); j < result.size(); ++j) {
                    if (result[j] != std::numeric_limits<data_type>::max()) {
                        ++result[j];
                        carry = false;
                        break;
                    }
                    result[j] = 0;
                }

                if (carry)
                    result.push_back(1);
            }
        }
    }
} // namespace numeric::algorithms
