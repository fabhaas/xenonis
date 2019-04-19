// ---------- (C) 2018, 2019 fahaas ----------
/*!
 *	\file bigint.hpp
 *	\brief A definition of the bigint class.
 *  \details NOTE: Requires at least a 64-bit unsigned integer resulting in no support for 32-bit builds.
 */
#pragma once

#include "bigint_config.hpp"

#include "algorithms/arithmetic.hpp"
#include "algorithms/compare.hpp"
#include "algorithms/conversion.hpp"
#include "container/bigint_data.hpp"
#include "integer_traits.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace xenonis {
    template <typename Value, class Container, std::uint8_t base_pow> class bigint;

    template <typename Value, class Container> class bigint<Value, Container, 16> {
        static_assert(std::is_integral<Value>::value && std::is_unsigned<Value>::value,
                      "Only unsigned integers are supported");
#ifndef XENONIS_USE_UINT128
        static_assert(!std::is_same<Value, std::uint64_t>::value,
                      "Requires extension which has to be supported by the compiler (supported by "
                      "both Clang and GCC)");
#endif
      protected:
        Container m_data;
        bool m_sign = false;
        using size_type = decltype(m_data.size());
        using base_type = typename traits::uint<Value>::doubled;
        constexpr static Value base_min_one = std::numeric_limits<Value>::max();
        constexpr static base_type base = static_cast<base_type>(base_min_one) + 1;
        bigint() {}
        bigint(Container data, bool sign = false) : m_data(std::move(data)), m_sign(sign) {}

      public:
        bigint(Value n) noexcept
        {
            m_data = Container(1);
            m_data.front() = n;
        }

        bigint(const bigint& other) : m_data(other.m_data), m_sign(other.m_sign) {}

        bigint(bigint&& other) : m_data(std::move(other.m_data)), m_sign(other.m_sign) {}

        bigint& operator=(const bigint& other) noexcept
        {
            m_data = other.m_data;
            m_sign = other.m_sign;
            return *this;
        }

        bigint& operator=(bigint&& other) noexcept
        {
            m_data = std::move(other.m_data);
            m_sign = other.m_sign;
            return *this;
        }

        /*bigint(std::uint64_t n) noexcept
        {
            if constexpr (!std::is_same<Value, std::uint64_t>::value) {
                // would be inefficient to use bitwise operators, because
                // the process may add redundant zeros requiring a call of remove_zeros() afterwards
                while (n != 0) {
                    m_data.push_back(static_cast<Value>(n % base));
                    n -= n % base;
                    n /= base;
                }
            } else {
                m_data.push_back(n);
            }
        }

        bigint(std::int64_t n) noexcept
        {
            if (n == 0) {
                m_data.push_back(0);
                m_sign = false;
                return;
            }

            std::uint64_t u;
            if (n < 0) {
                m_sign = true;
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

            if constexpr (!std::is_same<Value, std::uint64_t>::value) {
                // would be inefficient to use bitwise operators, because
                // the process may add redundant zeros requiring a call of remove_zeros() afterwards
                while (u != 0) {
                    m_data.push_back(static_cast<Value>(u % base));
                    u -= u % base;
                    u /= base;
                }
            } else {
                m_data.push_back(u);
            }
        }*/

        bigint(const std::string_view hex_str)
        {
            if ((hex_str.size() == 2 && hex_str.front() == '-' && hex_str.back() == '0') ||
                (hex_str.size() == 1 && hex_str.front() == '0')) {
                m_sign = false;
                m_data = Container(1, 0);
                return;
            }

            if (hex_str.empty())
                throw std::invalid_argument("Input string not valid!: hex_str.empty()");

            if (hex_str.front() == '-')
                m_sign = true;

            m_data = algorithms::hex_from_string<Value, Container>(
                std::string_view(hex_str.data() + m_sign, hex_str.size() - m_sign));
        }

        bigint& operator++()
        {
            if (m_sign) {
                if (m_data.size() == 1 && m_data.front() == 1) {
                    m_data.front() = 0;
                    m_sign = false;
                    return;
                }
                algorithms::hex_decrement(m_data.begin(), m_data.end());
            } else {
                if (algorithms::hex_increment(m_data.begin(), m_data.end()))
                    m_data.push_back(1);
            }
        }

        bigint& operator--()
        {
            if (!m_sign) {
                if (m_data.size() == 1 && m_data.front() == 0) {
                    m_data.front() = 1;
                    m_sign = true;
                    return;
                }
                algorithms::hex_decrement(m_data.begin(), m_data.end());
            } else {
                if (algorithms::hex_increment(m_data.begin(), m_data.end()))
                    m_data.push_back(1);
            }
        }

        bigint& operator++(int)
        {
            auto tmp = *this;
            ++tmp;
            return tmp;
        }

        bigint& operator--(int)
        {
            auto tmp = *this;
            --tmp;
            return tmp;
        }

        bigint& operator+=(const bigint& other)
        {
            auto add = [](const auto& a, const auto& b) {
                Container tmp(a.size() + 1);
                tmp.back() = 0;
                if (algorithms::hex_add(a.cbegin(), b.cbegin(), b.cend(), tmp.begin())) {
                    std::copy(a.cbegin() + b.size(), a.cend(), tmp.begin() + b.size());
                    algorithms::hex_increment(tmp.begin() + a.size(), tmp.end());
                } else {
                    std::copy(a.begin() + b.size(), a.end(), tmp.begin() + b.size());
                    tmp.pop_back();
                }
                return std::move(tmp);
            };

            if (m_sign == other.m_sign) {
                if (m_data.size() >= other.m_data.size()) {
                    if (m_data.size() - 1 >= other.m_data.size()) {
                        if (algorithms::hex_add(m_data.cbegin(), other.m_data.cbegin(), other.m_data.cend(),
                                                m_data.begin())) {
                            if (algorithms::hex_increment(m_data.begin() + other.m_data.size(), m_data.end()))
                                m_data.push_back(1);
                        }
                    } else {
                        m_data = add(m_data, other.m_data);
                    }
                } else {
                    m_data = add(other.m_data, m_data);
                }
            } else { // is_signed != other.is_signed
                if (algorithms::greater<Container>(m_data, other.m_data)) {
                    if (algorithms::hex_sub_from(m_data.begin(), other.m_data.cbegin(), other.m_data.cend()))
                        algorithms::hex_decrement(m_data.begin() + other.m_data.size(), m_data.end());
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    Container tmp(other.m_data.size());
                    if (algorithms::hex_sub(other.m_data.cbegin(), m_data.cbegin(), m_data.cend(), tmp.begin())) {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                        algorithms::hex_decrement(tmp.begin() + m_data.size(), tmp.end());
                    } else {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                    }
                    m_data = std::move(tmp);
                    m_sign = !m_sign;
                }
                algorithms::remove_zeros(m_data);
            }

            if (m_data.size() == 1 && m_data.front() == 0)
                m_sign = false;

            return *this;
        }

        bigint& operator-=(const bigint& other)
        {
            // because other is read-only and a copy is expensive, operator-= is implemented without
            // just other.is_signed = !other.is_signed; and calling operator+=
            auto add = [](const auto& a, const auto& b) {
                Container tmp(a.size() + 1);
                tmp.back() = 0;
                if (algorithms::hex_add(a.cbegin(), b.cbegin(), b.cend(), tmp.begin())) {
                    std::copy(a.cbegin() + b.size(), a.cend(), tmp.begin() + b.size());
                    algorithms::hex_increment(tmp.begin() + a.size(), tmp.end());
                } else {
                    std::copy(a.begin() + b.size(), a.end(), tmp.begin() + b.size());
                    tmp.pop_back();
                }
                return std::move(tmp);
            };

            if (m_sign != other.m_sign) {
                if (m_data.size() >= other.m_data.size()) {
                    if (m_data.size() - 1 >= other.m_data.size()) {
                        if (algorithms::hex_add(m_data.cbegin(), other.m_data.cbegin(), other.m_data.cend(),
                                                m_data.begin())) {
                            if (algorithms::hex_increment(m_data.begin() + other.m_data.size(), m_data.end()))
                                m_data.push_back(1);
                        }
                    } else {
                        m_data = add(m_data, other.m_data);
                    }
                } else {
                    m_data = add(other.m_data, m_data);
                }
            } else { // is_signed != other.is_signed
                if (algorithms::greater<Container>(m_data, other.m_data)) {
                    if (algorithms::hex_sub_from(m_data.begin(), other.m_data.cbegin(), other.m_data.cend()))
                        algorithms::hex_decrement(m_data.begin() + other.m_data.size(), m_data.end());
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    Container tmp(other.m_data.size());
                    if (algorithms::hex_sub(other.m_data.cbegin(), m_data.cbegin(), m_data.cend(), tmp.begin())) {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                        algorithms::hex_decrement(tmp.begin() + m_data.size(), tmp.end());
                    } else {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                    }
                    m_data = std::move(tmp);
                    m_sign = !m_sign;
                }
                algorithms::remove_zeros(m_data);
            }

            if (m_data.size() == 1 && m_data.front() == 0)
                m_sign = false;
            return *this;
        }

        bigint& operator*=(const bigint& other)
        {
            if (m_data.size() == 1 && m_data.front() == 0)
                return *this;

            if (other.m_data.size() == 1 && other.m_data.front() == 0) {
                m_data = Container(1, 0);
                m_sign = false;
                return *this;
            }

            m_data = algorithms::hex_karatsuba_mul<Container>(m_data.cbegin(), m_data.cend(), other.m_data.cbegin(),
                                                              other.m_data.cend());
            m_sign = m_sign != other.m_sign;
            return *this;
        }

        bigint& operator/=(const bigint& other) noexcept;

#define BIGINT_ARITHMETIC_OPERTATOR_IMPL(op)                                                                           \
    bigint operator op(const bigint& other) const                                                                      \
    {                                                                                                                  \
        auto tmp = *this;                                                                                              \
        tmp op## = other;                                                                                              \
        return tmp;                                                                                                    \
    }

        BIGINT_ARITHMETIC_OPERTATOR_IMPL(+)
        BIGINT_ARITHMETIC_OPERTATOR_IMPL(-)
        BIGINT_ARITHMETIC_OPERTATOR_IMPL(*)
        // BIGINT_ARITHMETIC_OPERTATOR_IMPL(/)

#undef BIGINT_ARITHMETIC_OPERTATOR_IMPL

        bool operator<(const bigint& other) const noexcept
        {
            if (m_sign == other.m_sign)
                return m_sign ? algorithms::greater(m_data, other.m_data, false)
                              : algorithms::less(m_data, other.m_data, false);
            else
                return m_sign;
        }

        bool operator>(const bigint& other) const noexcept
        {
            if (m_sign == other.m_sign)
                return m_sign ? algorithms::less(m_data, other.m_data, false)
                              : algorithms::greater(m_data, other.m_data, false);
            else
                return !m_sign;
        }

        bool operator==(const bigint& other) const noexcept { return m_sign == other.m_sign && m_data == other.m_data; }

        bool operator!=(const bigint& other) const noexcept { return !this->operator==(other); }

        bool operator<=(const bigint& other) const noexcept
        {
            if (m_sign == other.m_sign)
                return m_sign ? algorithms::greater(m_data, other.m_data, true)
                              : algorithms::less(m_data, other.m_data, true);
            else
                return m_sign;
        }

        bool operator>=(const bigint& other) const noexcept
        {
            if (m_sign == other.m_sign)
                return m_sign ? algorithms::less(m_data, other.m_data, true)
                              : algorithms::greater(m_data, other.m_data, true);
            else
                return !m_sign;
        }

        std::string to_string(bool lower_case = true) const
        {
            return algorithms::hex_to_string<Value, Container>(m_data, m_sign, lower_case);
        }

        inline size_type size() const noexcept { return m_data.size() * sizeof(Value); }
        const Container& data() const noexcept { return m_data; }

        virtual ~bigint() = default;
    };

    // operator implementations
    template <typename Value, class Container, std::uint8_t base_pow>
    std::ostream& operator<<(std::ostream& out, const bigint<Value, Container, base_pow>& b)
    {
        return out << b.to_string();
    }

    using hex_bigint64 = xenonis::bigint<std::uint64_t, internal::bigint_data<std::uint64_t>, 16>;
    using hex_bigint32 = xenonis::bigint<std::uint32_t, internal::bigint_data<std::uint32_t>, 16>;
    using hex_bigint16 = xenonis::bigint<std::uint16_t, internal::bigint_data<std::uint16_t>, 16>;
    using hex_bigint8 = xenonis::bigint<std::uint8_t, internal::bigint_data<std::uint8_t>, 16>;
    using hex_bigint = hex_bigint64; // default
} // namespace xenonis
