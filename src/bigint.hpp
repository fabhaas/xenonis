//(C) 2018 fahaas
/*!
 *	\file bigint.hpp
 *	\brief A definition of the bigint class.
 *  \details NOTE: Requires at least a 64-bit unsigned integer resulting in no support for 32-bit builds.
 */
#pragma once

#include <boost/integer.hpp>
#include <boost/operators.hpp>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace numeric {

#ifdef GNUC
    using uint128_t = unsigned __int128; //!< unsigned 128-bit integer
#endif

    template <typename data_type, class vector_allocator = std::allocator<data_type>>
    class bigint : boost::arithmetic<bigint<data_type, vector_allocator>>,
                   boost::unit_steppable<bigint<data_type, vector_allocator>> {
        static_assert(std::is_integral<data_type>::value && std::is_unsigned<data_type>::value,
                      "Only unsigned integers are supported");
#ifndef GNUC
        static_assert(!std::is_same<data_type, std::uint64_t>::value, "Not supported when not using gcc or clang");
#endif
      protected:
        using data_container_type = std::vector<data_type, vector_allocator>;                //!< the type of data
        using base_type = typename boost::uint_t<sizeof(data_type) * 16 /* = 2*8 */>::exact; //!< the type of base
        data_container_type data; //!< container containing the data

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

        bool is_signed = false; //!< is true if the represented number is signed, else false
        constexpr static base_type base =
            static_cast<base_type>(std::numeric_limits<data_type>::max()) + 1; //!< the base of the number system
        constexpr static data_type base_min_one =
            std::numeric_limits<data_type>::max(); //!< the base-1 of the number system

      public:
        using size_type = decltype(data.size()); //!< the size type used by this class
        bigint(data_type n) noexcept;
        bigint(std::int64_t n) noexcept;
        bigint(std::string hex_str);
        bigint(const bigint& other) : data(other.data), is_signed(other.is_signed) {}
        bigint& operator++() noexcept;
        bigint& operator--() noexcept;
        bigint& operator+=(bigint other) noexcept;
        bigint& operator-=(bigint other) noexcept;
        bigint& operator*=(bigint other) noexcept;
        bigint& operator/=(bigint other) noexcept;
        bool operator<(const bigint& other) const noexcept;
        bool operator>(const bigint& other) const noexcept;
        bool operator==(const bigint& other) const noexcept;
        bool operator!=(const bigint& other) const noexcept;
        bool operator<=(const bigint& other) const noexcept;
        bool operator>=(const bigint& other) const noexcept;
        std::string to_string() const noexcept;
        inline size_type size();
        virtual ~bigint() = default;
    };

    template <typename data_type, class vector_allocator = std::allocator<data_type>>
    std::ostream& operator<<(std::ostream& out, const bigint<data_type, vector_allocator>& b) {
        return out << b.to_string();
    }

    template class bigint<std::uint8_t>;
    template class bigint<std::uint16_t>;
    template class bigint<std::uint32_t>;
#ifdef GNUC
    template class bigint<std::uint64_t>; // only supported when using gcc or clang
#endif                                  

} // namespace numeric
