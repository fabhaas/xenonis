// ---------- (C) 2018-2020 Fabian Haas ----------
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

namespace xenonis::internal {
    template <typename Value, class Container> class bigint {
        static_assert(std::is_integral<Value>::value && std::is_unsigned<Value>::value,
                      "Only unsigned integers are supported");
#ifndef XENONIS_USE_UINT128
        static_assert(!std::is_same<Value, std::uint64_t>::value,
                      "Requires extension which has to be supported by the compiler (supported by "
                      "both Clang and GCC)");
#endif
      private:
        Container m_data;
        bool m_sign{false};
        using size_type = decltype(m_data.size());
        using base_type = typename traits::uinteger<Value>::doubled;
        constexpr static Value base_min_one{std::numeric_limits<Value>::max()};
        constexpr static base_type base{static_cast<base_type>(base_min_one) + 1};
        bigint(Container data, bool sign = false) : m_data(std::move(data)), m_sign(sign) {}

      public:
        bigint() noexcept {}

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

#define BIGINT_INT_CONSTRUCTOR(T)                                                                                      \
    bigint(T n) noexcept                                                                                               \
    {                                                                                                                  \
        auto ret{algorithms::from_int<Value, T, Container>(n)};                                                        \
        m_sign = ret.first;                                                                                            \
        m_data = ret.second;                                                                                           \
    }

        BIGINT_INT_CONSTRUCTOR(std::int64_t)
        BIGINT_INT_CONSTRUCTOR(std::int32_t)
        BIGINT_INT_CONSTRUCTOR(std::int16_t)
        BIGINT_INT_CONSTRUCTOR(std::int8_t)
#undef BIGINT_INT_CONSTRUCTOR

#define BIGINT_UINT_CONSTRUCTOR(T)                                                                                     \
    bigint(T n) noexcept { m_data = algorithms::from_uint<Value, T, Container>(n); }

        BIGINT_UINT_CONSTRUCTOR(std::uint64_t)
        BIGINT_UINT_CONSTRUCTOR(std::uint32_t)
        BIGINT_UINT_CONSTRUCTOR(std::uint16_t)
        BIGINT_UINT_CONSTRUCTOR(std::uint8_t)
#undef BIGINT_UINT_CONSTRUCTOR

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

            m_data = algorithms::from_string<Value, Container>(
                std::string_view(hex_str.data() + m_sign, hex_str.size() - m_sign));
        }

        bigint& operator++()
        {
            if (m_sign) {
                if (m_data.size() == 1 && m_data.front() == 1) {
                    m_data.front() = 0;
                    m_sign = false;
                    return *this;
                }
                algorithms::decrement(m_data.begin(), m_data.end());
            } else {
                if (algorithms::increment(m_data.begin(), m_data.end()))
                    m_data.push_back(1);
            }
            return *this;
        }

        bigint& operator--()
        {
            if (!m_sign) {
                if (m_data.size() == 1 && m_data.front() == 0) {
                    m_data.front() = 1;
                    m_sign = true;
                    return *this;
                }
                algorithms::decrement(m_data.begin(), m_data.end());
            } else {
                if (algorithms::increment(m_data.begin(), m_data.end()))
                    m_data.push_back(1);
            }
            return *this;
        }

        bigint& operator++(int)
        {
            auto tmp{*this};
            ++tmp;
            return tmp;
        }

        bigint& operator--(int)
        {
            auto tmp{*this};
            --tmp;
            return tmp;
        }

        bigint& operator+=(const bigint& other)
        {
            constexpr auto add = [](const auto& a, const auto& b) {
                Container tmp(a.size() + 1);
                tmp.back() = 0;
                if (algorithms::add(a.cbegin(), b.cbegin(), b.cend(),
                                    tmp.begin())) { // algorithms::add(a.cbegin(), b.cbegin(), tmp.begin(), b.size())
                    std::copy(a.cbegin() + b.size(), a.cend(), tmp.begin() + b.size());
                    algorithms::increment(tmp.begin() + a.size(), tmp.end());
                } else {
                    std::copy(a.begin() + b.size(), a.end(), tmp.begin() + b.size());
                    tmp.pop_back();
                }
                return tmp;
            };

            if (m_sign == other.m_sign) {
                if (m_data.size() >= other.m_data.size()) {
                    if (m_data.size() - 1 >= other.m_data.size()) {
                        if (algorithms::add(
                                m_data.cbegin(), other.m_data.cbegin(), other.m_data.cend(),
                                m_data.begin())) { // algorithms::add(m_data.cbegin(), other.m_data.cbegin(),
                                                   // m_data.begin(), other.m_data.size())
                            if (algorithms::increment(m_data.begin() + other.m_data.size(), m_data.end()))
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
                    if (algorithms::sub_from(m_data.begin(), other.m_data.cbegin(), other.m_data.cend()))
                        algorithms::decrement(m_data.begin() + other.m_data.size(), m_data.end());
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    Container tmp(other.m_data.size());
                    if (algorithms::sub(other.m_data.cbegin(), m_data.cbegin(), m_data.cend(), tmp.begin())) {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                        algorithms::decrement(tmp.begin() + m_data.size(), tmp.end());
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
            constexpr auto add = [](const auto& a, const auto& b) {
                Container tmp(a.size() + 1);
                tmp.back() = 0;
                if (algorithms::add(a.cbegin(), b.cbegin(), b.cend(), tmp.begin())) {
                    std::copy(a.cbegin() + b.size(), a.cend(), tmp.begin() + b.size());
                    algorithms::increment(tmp.begin() + a.size(), tmp.end());
                } else {
                    std::copy(a.begin() + b.size(), a.end(), tmp.begin() + b.size());
                    tmp.pop_back();
                }
                return /*std::move(*/ tmp /*)*/;
            };

            if (m_sign != other.m_sign) {
                if (m_data.size() >= other.m_data.size()) {
                    if (m_data.size() > other.m_data.size()) {
                        if (algorithms::add(m_data.cbegin(), other.m_data.cbegin(), other.m_data.cend(),
                                            m_data.begin())) {
                            if (algorithms::increment(m_data.begin() + other.m_data.size(), m_data.end()))
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
                    if (algorithms::sub_from(m_data.begin(), other.m_data.cbegin(), other.m_data.cend()))
                        algorithms::decrement(m_data.begin() + other.m_data.size(), m_data.end());
                    // is_signed = is_signed; when is_signed == true then the result has to be <= 0,
                    // else >= 0
                } else {
                    Container tmp(other.m_data.size());
                    if (algorithms::sub(other.m_data.cbegin(), m_data.cbegin(), m_data.cend(), tmp.begin())) {
                        std::copy(other.m_data.cbegin() + m_data.size(), other.m_data.cend(),
                                  tmp.begin() + m_data.size());
                        algorithms::decrement(tmp.begin() + m_data.size(), tmp.end());
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

            m_data = algorithms::naive_mul<Container>(m_data.cbegin(), m_data.cend(), other.m_data.cbegin(),
                                                      other.m_data.cend());

            m_sign = m_sign != other.m_sign;
            return *this;
        }

        bigint& operator/=(const bigint& other) noexcept;

#define BIGINT_ARITHMETIC_OPERTATOR_IMPL(op)                                                                           \
    bigint operator op(const bigint& other) const                                                                      \
    {                                                                                                                  \
        auto tmp{*this};                                                                                               \
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
            return algorithms::to_string<Value, Container>(m_data, m_sign, lower_case);
        }

        inline size_type size() const noexcept { return m_data.size() * sizeof(Value); }
        const Container& data() const noexcept { return m_data; }

        virtual ~bigint() = default;
    };

    // operator implementations
    template <typename Value, class Container>
    std::ostream& operator<<(std::ostream& out, const bigint<Value, Container>& b)
    {
        return out << b.to_string();
    }
} // namespace xenonis::internal

namespace xenonis {
#ifdef XENONIS_USE_UINT128
    using bigint64 = internal::bigint<std::uint64_t, internal::bigint_data<std::uint64_t>>;
#endif
    using bigint32 = internal::bigint<std::uint32_t, internal::bigint_data<std::uint32_t>>;
    using bigint16 = internal::bigint<std::uint16_t, internal::bigint_data<std::uint16_t>>;
    using bigint8 = internal::bigint<std::uint8_t, internal::bigint_data<std::uint8_t>>;
    using bigint = std::conditional<std::is_same_v<typename traits::uinteger<std::uintmax_t>::doubled, void>,
                                    internal::bigint<std::uintmax_t, internal::bigint_data<std::uintmax_t>>, bigint32>;
} // namespace xenonis
