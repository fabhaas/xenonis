// ---------- (C) 2018 fahaas ----------
/*!
 *	\file bigint.hpp
 *	\brief A definition of the bigint class.
 *  \details NOTE: Requires at least a 64-bit unsigned integer resulting in no support for 32-bit
 *builds.
 */
#pragma once

#include "bigint_config.hpp"

#include "algorithms.hpp"
#include "integer_traits.hpp"
#include <algorithm>
#include <array>
#include <boost/operators.hpp>
#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace xenonis {
#ifdef XENONIS_USE_UINT128
    using uint128_t = unsigned __int128; //!< unsigned 128-bit integer
#endif
    template <typename data_type, class container_type, std::uint8_t base_pow> class bigint;

    template <typename data_type, class container_type>
    class bigint<data_type, container_type, 16>
        : boost::addable<bigint<data_type, container_type, 16>>,
          boost::subtractable<bigint<data_type, container_type, 16>> {
        static_assert(std::is_integral<data_type>::value && std::is_unsigned<data_type>::value,
                      "Only unsigned integers are supported");
#ifndef XENONIS_USE_UINT128
        static_assert(!std::is_same<data_type, std::uint64_t>::value,
                      "Not supported when not using GCC or Clang");
#endif
      protected:
        container_type data;
        bool is_signed = false;
        using size_type = decltype(data.size());
        using base_type = typename traits::get_doubled<data_type>::type;
        constexpr static data_type base_min_one = std::numeric_limits<data_type>::max();
        constexpr static base_type base = static_cast<base_type>(base_min_one) + 1;
        bigint() {}
        bigint(container_type data, bool is_signed = false)
            : data(std::move(data)), is_signed(is_signed)
        {
        }

      public:
        /*bigint(data_type n) noexcept
          {
              data = container_type(1);
              data[0] = n;
          }*/

        bigint(const bigint& other) : data(other.data), is_signed(other.is_signed) {}

        bigint(bigint&& other) : data(std::move(other.data)), is_signed(other.is_signed) {}

        bigint& operator=(const bigint& other)
        {
            data = other.data;
            is_signed = other.is_signed;
            return *this;
        }

        bigint& operator=(bigint&& other)
        {
            data = std::move(other.data);
            is_signed = other.is_signed;
            return *this;
        }

        bigint(std::uint64_t n) noexcept
        {
            if constexpr (!std::is_same<data_type, std::uint64_t>::value) {
                // would be inefficient to use bitwise operators, because
                // the process may add redundant zeros requiring a call of remove_zeros() afterwards
                while (n != 0) {
                    data.push_back(static_cast<data_type>(n % base));
                    n -= n % base;
                    n /= base;
                }
            } else {
                data.push_back(n);
            }
        }

        bigint(std::int64_t n) noexcept
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
                } else {
                    u = static_cast<std::uint64_t>(n * -1);
                }
            } else {
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
            } else {
                data.push_back(u);
            }
        }

        bigint(const std::string_view hex_str)
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
                    throw std::invalid_argument("Input string not valid!: invalid char");
            };

            constexpr std::uint8_t block_size = sizeof(data_type) * 2;
            data.resize(hex_str.size() / block_size + (hex_str.size() % block_size == 0 ? 0 : 1),
                        0);

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

        bigint& operator++() noexcept {}

        bigint& operator--() noexcept {}

        bigint& operator+=(const bigint& other) noexcept
        {
            if (is_signed == other.is_signed) {
                if (size() >= other.size())
                    data = algorithms::hex_add<container_type>(data, other.data);
                else
                    data = algorithms::hex_add<container_type>(other.data, data);
            } else { // is_signed != other.is_signed
                if (algorithms::greater<container_type>(data, other.data)) {
                    data = algorithms::hex_sub<container_type>(data, other.data);
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    data = algorithms::hex_sub<container_type>(other.data, data);
                    is_signed = !is_signed;
                }
            }

            return *this;
        }

        bigint& operator-=(const bigint& other) noexcept
        {
            // because other is read-only and a copy is expensive, operator-= is implemented without
            // just other.is_signed = !other.is_signed; and calling operator+=
            if (is_signed != other.is_signed) {
                if (size() >= other.size())
                    data = algorithms::hex_add<container_type>(data, other.data);
                else
                    data = algorithms::hex_add<container_type>(other.data, data);
            } else {
                if (algorithms::greater<container_type>(data, other.data)) {
                    data = algorithms::hex_sub<container_type>(data, other.data);
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    data = algorithms::hex_sub<container_type>(other.data, data);
                    is_signed = !is_signed;
                }
            }

            return *this;
        }

        bigint& operator*=(const bigint& other) noexcept;

        bigint& operator/=(const bigint& other) noexcept;

        bool operator<(const bigint& other) const noexcept
        {
            if (is_signed == other.is_signed) {
                if (data.size() < other.data.size())
                    return !is_signed;
                if (data.size() > other.data.size())
                    return is_signed;

                for (size_type i = data.size() - 1; i < data.size();
                     --i) { // because std::size_t flips
                    if (other.data[i] > data[i])
                        return !is_signed;
                    if (data[i] > other.data[i])
                        return is_signed;
                }
                return false; // data == other.data
            } else {
                return is_signed;
            }
        }

        bool operator>(const bigint& other) const noexcept
        {
            if (is_signed == other.is_signed) {
                if (data.size() < other.data.size())
                    return is_signed;
                if (data.size() > other.data.size())
                    return !is_signed;

                for (std::size_t i = data.size() - 1; i < data.size(); --i) {
                    if (other.data[i] > data[i])
                        return is_signed;
                    if (data[i] > other.data[i])
                        return !is_signed;
                }
                return false; // data == other.data
            } else {
                return !is_signed;
            }
        }

        bool operator==(const bigint& other) const noexcept
        {
            return is_signed == other.is_signed && data == other.data;
        }

        bool operator!=(const bigint& other) const noexcept { return !this->operator==(other); }

        bool operator<=(const bigint& other) const noexcept
        {
            if (is_signed == other.is_signed) {
                if (data.size() < other.data.size())
                    return !is_signed;
                if (data.size() > other.data.size())
                    return is_signed;

                for (size_type i = data.size() - 1; i < data.size();
                     --i) { // because std::size_t flips
                    if (other.data[i] > data[i])
                        return !is_signed;
                    if (data[i] > other.data[i])
                        return is_signed;
                }
                return true; // data == other.data
            } else {
                return is_signed;
            }
        }

        bool operator>=(const bigint& other) const noexcept
        {
            if (is_signed == other.is_signed) {
                if (data.size() < other.data.size())
                    return is_signed;
                if (data.size() > other.data.size())
                    return !is_signed;

                for (std::size_t i = data.size() - 1; i < data.size(); --i) {
                    if (other.data[i] > data[i])
                        return is_signed;
                    if (data[i] > other.data[i])
                        return !is_signed;
                }
                return true; // data == other.data
            } else {
                return !is_signed;
            }
        }

        std::string to_string(bool lower_case = true) const noexcept
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

        inline size_type size() const { return data.size() * sizeof(data_type); }

        virtual ~bigint() = default;
    };

    // operator implementations
    template <typename data_type, class container_type, std::uint8_t base_pow>
    std::ostream& operator<<(std::ostream& out,
                             const bigint<data_type, container_type, base_pow>& b)
    {
        return out << b.to_string();
    }

    using hex_bigint64 = xenonis::bigint<std::uint64_t, std::vector<std::uint64_t>, 16>;
    using hex_bigint32 = xenonis::bigint<std::uint64_t, std::vector<std::uint32_t>, 16>;
    using hex_bigint16 = xenonis::bigint<std::uint64_t, std::vector<std::uint16_t>, 16>;
    using hex_bigint8 = xenonis::bigint<std::uint64_t, std::vector<std::uint8_t>, 16>;
    using hex_bigint = hex_bigint64; //default
} // namespace xenonis
