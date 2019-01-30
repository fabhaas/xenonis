// ---------- (C) 2018 fahaas ----------
/*!
 *	\file bigint.cpp
 *	\brief implementation of bigint class
 */

#include "bigint.hpp"
#include "algorithms.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

namespace numeric {
    template <typename data_type, class allocator> bigint<data_type, allocator>::bigint(std::uint64_t n) noexcept
    {
        if constexpr (!std::is_same<data_type, std::uint64_t>::value) {
            // would be inefficient to use bitwise operators, because
            // the process may add redundant zeros requiring a call of remove_zeros() afterwards
            while (n != 0) {
                data.push_back(static_cast<data_type>(n % base));
                n -= n % base;
                n /= base;
            }
        }
        else {
            data.push_back(n);
        }
    }

    template <typename data_type, class allocator> bigint<data_type, allocator>::bigint(std::int64_t n) noexcept
    {
        if (n == 0) {
            data.push_back(0);
            is_signed = false;
            return;
        }

        std::uint64_t u;
        if (n < 0) {
            is_signed = true;
            if (n == std::numeric_limits<std::int64_t>::min()) {
                --n;
                n *= -1;
                u = static_cast<std::uint64_t>(n);
                ++u;
            }
            else {
                u = static_cast<std::uint64_t>(n * -1);
            }
        }
        else {
            u = static_cast<std::uint64_t>(n);
        }

        if constexpr (!std::is_same<data_type, std::uint64_t>::value) {
            // would be inefficient to use bitwise operators, because
            // the process may add redundant zeros requiring a call of remove_zeros() afterwards
            while (u != 0) {
                data.push_back(static_cast<data_type>(u % base));
                u -= u % base;
                u /= base;
            }
        }
        else {
            data.push_back(u);
        }
    }

    template <typename data_type, class allocator> bigint<data_type, allocator>::bigint(const std::string_view hex_str)
    {
        if (hex_str.empty())
            throw std::invalid_argument("Input string not valid!: hex_str.empty()");

        if (hex_str.front() == '-')
            is_signed = true;

        auto conv = [](auto& c) {
            if (c >= 48 && c <= 57)
                return c - 48;
            else if (c >= 65 && c <= 90)
                return c - 55;
            else if (c >= 97 && c <= 122)
                return c - 87;
            else
                throw std::invalid_argument("Input string not valid!");
        };

        constexpr std::uint8_t block_size = sizeof(data_type) * 2;
        data.resize(hex_str.size() / block_size + (hex_str.size() % block_size == 0 ? 0 : 1), 0);

        size_type i_data = 0;
        size_type i_block = 0;
        auto first = hex_str.crbegin();
        auto last = hex_str.crend() - static_cast<std::size_t>(is_signed);
        for (; first != last; ++first) {
            data[i_data] |= (static_cast<std::uint64_t>(conv(*first)) << (4 * (i_block++)));

            if (i_block == block_size) {
                i_block = 0;
                ++i_data;
            }
        }
    }

    template <typename data_type, class allocator>
    bigint<data_type, allocator>& bigint<data_type, allocator>::operator*=(const bigint& other) noexcept
    {
        /*
        //currently not working
        auto get_p2_size = [] (size_type size) -> size_type {
            size_type i = 2;
            while (i <= size)
                i <<= 1;
            return i;
        };

        size_type a_size = get_p2_size(data.size());
        size_type b_size = get_p2_size(data.size());

        if (a_size < b_size)
            while (a_size != b_size)
                a_size <<= 1;
        else if(a_size > b_size)
            while (a_size != b_size)
                b_size <<= 1;

        container_type a(a_size, 0);
        container_type b(b_size, 0);

        std::copy(data.begin(), data.end(), a.begin());
        std::copy(other.data.begin(), other.data.end(), b.begin());

        data = algorithms::mult<data_type>(std::move(a), std::move(b));*/

        std::vector<container_type> results(other.data.size());

        //#pragma omp parallel for
        for (size_type i = 0; i < other.data.size(); ++i)
            results[i] = algorithms::rshift<data_type>(algorithms::mult_with_digit<data_type>(data, other.data[i]), i);

        algorithms::sum_vec<data_type>(results, data);

        is_signed = is_signed != other.is_signed;

        return *this;
    }

    template <typename data_type, class allocator>
    bigint<data_type, allocator>& bigint<data_type, allocator>::operator+=(bigint other) noexcept
    {
        auto make_complement = [](container_type& data) {
            // for base complement
            std::size_t d_size = data.size() + 1; // because / always floor(), + 1
            container_type b;

            // make base complement
            b.resize(d_size, base_min_one);

            for (size_type i = 0; i < data.size(); ++i)
                b[i] -= data[i];

            for (size_type i = 0; i < data.size(); ++i) {
                if (b[i] != base_min_one) {
                    ++b[i];
                    break;
                }
                b[i] = 0;
            }

            // copy data
            data.resize(b.size());
            std::copy(b.begin(), b.end(), data.begin());
        };

        auto undo_complement = [](container_type& data) {
            container_type b;
            b.resize(data.size(), base_min_one);

            for (size_type i = 0; i < data.size(); ++i)
                b[i] -= data[i];

            for (size_type i = 0; i < b.size(); ++i) {
                if (b[i] != base_min_one) {
                    ++b[i];
                    break;
                }
                b[i] = 0;
            }

            data = b;
            algorithms::remove_zeros(data);
        };

        if (is_signed)
            make_complement(data);
        else
            data.push_back(0);
        if (other.is_signed)
            make_complement(other.data);
        else
            other.data.push_back(0);

        if (data.size() != other.data.size()) {
            if (data.size() < other.data.size())
                data.resize(other.data.size(), is_signed ? base_min_one : 0);
            else
                other.data.resize(data.size(), other.is_signed ? base_min_one : 0);
        }

        assert(data.size() == other.data.size());
        assert(data.back() == (is_signed ? base_min_one : 0));
        assert(other.data.back() == (other.is_signed ? base_min_one : 0));

        container_type result(data.size(), 0);

        // add
        algorithms::add<data_type>(data, other.data, result);

        data.resize(result.size());
        std::copy(result.begin(), result.end(), data.begin());

        if (is_signed || other.is_signed) {
            switch (data.back()) {
            case base_min_one:
            case base_min_one - 1:
                is_signed = true;
                break;

            case 0:
            case 1:
                is_signed = false;
                break;
            }
        }

        algorithms::remove_zeros(data);

        if (is_signed)
            undo_complement(data);

        if (data.size() == 1 && data[0] == 0)
            is_signed = false;

        return *this;
    }

    template <typename data_type, class allocator>
    std::string bigint<data_type, allocator>::to_string(bool lower_case) const noexcept
    {
        std::string ret(static_cast<size_type>(data.size() * sizeof(data_type) * 2), '\0');

        // convert to hex
        constexpr std::uint8_t mask0 = 0xF0;
        constexpr std::uint8_t mask1 = 0xF;

        size_type i_ret = 0;
        std::for_each(data.begin(), data.end(), [&ret, &i_ret](data_type n) {
            auto* n_ptr = reinterpret_cast<std::uint8_t*>(&n);

            for (size_type i = 0; i < sizeof(data_type); ++i) {
                ret[i_ret++] = static_cast<char>(mask1 & n_ptr[i]);
                ret[i_ret++] = static_cast<char>((mask0 & n_ptr[i]) >> 4);
            }
        });

        // remove zeros
        algorithms::remove_zeros(ret);

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

    template class bigint<std::uint8_t>;
    template class bigint<std::uint16_t>;
    template class bigint<std::uint32_t>;
#ifdef USE_UINT128
    template class bigint<std::uint64_t>; // only supported when using gcc or clang
#endif
} // namespace numeric
