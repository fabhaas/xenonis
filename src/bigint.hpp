// ---------- (C) 2018 fahaas ----------
/*!
 *	\file bigint.hpp
 *	\brief A definition of the bigint class.
 *  \details NOTE: Requires at least a 64-bit unsigned integer resulting in no support for 32-bit builds.
 */
#pragma once

#include "bigint_config.hpp"

#include "integer_traits.hpp"
#include <boost/operators.hpp>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace numeric {

#ifdef USE_UINT128
    using uint128_t = unsigned __int128; //!< unsigned 128-bit integer
#endif

    template <typename data_type, class allocator = std::allocator<data_type>>
    class bigint : boost::arithmetic<bigint<data_type, allocator>>,
                   boost::unit_steppable<bigint<data_type, allocator>> {
        static_assert(std::is_integral<data_type>::value && std::is_unsigned<data_type>::value,
                      "Only unsigned integers are supported");
#ifndef USE_UINT128
        static_assert(!std::is_same<data_type, std::uint64_t>::value, "Not supported when not using GCC or Clang");
#endif
      protected:
        using container_type = std::vector<data_type, allocator>;        //!< the type of data
        using base_type = typename traits::get_doubled<data_type>::type; //!< the type of base
        container_type data;                                             //!< container containing the data

        /*!
                \brief Complements the data.
                \details The complement is a complement on base.
                \sa base, undo_complement()
        */
        void make_complement() noexcept;

        /*!
                \brief Undos the complementation of the data which was complemented by make_complement().
                \sa make_complement()
        */
        void undo_complement() noexcept;

        /*!
                \brief Removes all redundant zeros.
        */
        void remove_zeros() noexcept;

        bigint() {}
        bigint(container_type data, bool is_signed = false) : data(std::move(data)), is_signed(is_signed) {}
        bool is_signed = false; //!< is true if the represented number is signed, else false
        constexpr static base_type base =
            static_cast<base_type>(std::numeric_limits<data_type>::max()) + 1; //!< the base of the number system
        constexpr static data_type base_min_one =
            std::numeric_limits<data_type>::max(); //!< the base-1 of the number system

      public:
        using size_type = decltype(data.size()); //!< the size type used by this class
        bigint(std::uint64_t n) noexcept;
        bigint(std::int64_t n) noexcept;
        bigint(const std::string_view hex_str);
        bigint(const bigint& other) : data(other.data), is_signed(other.is_signed) {}
        bigint& operator++() noexcept;
        bigint& operator--() noexcept;
        bigint& operator+=(bigint other) noexcept;
        bigint& operator-=(bigint other) noexcept;
        bigint& operator*=(const bigint& other) noexcept;
        bigint& operator/=(bigint other) noexcept;
        bool operator<(const bigint& other) const noexcept;
        bool operator>(const bigint& other) const noexcept;
        bool operator==(const bigint& other) const noexcept;
        bool operator!=(const bigint& other) const noexcept;
        bool operator<=(const bigint& other) const noexcept;
        bool operator>=(const bigint& other) const noexcept;
        std::string to_string(bool lower_case = true) const noexcept;
        inline size_type size();
        virtual ~bigint() = default;
    };

    template <typename data_type, class vector_allocator = std::allocator<data_type>>
    std::ostream& operator<<(std::ostream& out, const bigint<data_type, vector_allocator>& b)
    {
        return out << b.to_string();
    }
} // namespace numeric
