//(C) 2018 fahaas
/*!
*	\file bigint.hpp
*	\brief implementation of bigint class
*/
#pragma once

#include <limits>
#include <string>
#include <vector>
#include <utility>
#include <boost/integer.hpp>

namespace numeric {

#ifdef GNUC
	using uint128_t = unsigned __int128; /*! < unsigned 128-bit integer */
#endif // GNUC

	template <typename data_type, class vector_allocator = std::allocator<data_type>,
		typename = typename std::enable_if<
			std::is_integral<data_type>::value && std::is_unsigned<data_type>::value
#ifndef GNUC
		&& !std::is_same<data_type, std::uint64_t>::value //only supported when using gcc or clang
#endif // GNUC
			>::type>
		class bigint {
	protected:
		using data_container_type = std::vector<data_type, vector_allocator>;
		using base_type = typename boost::uint_t<sizeof(data_type) * 16 /* = 2*8 */>::exact;
		data_container_type data; /*! < container containing the data */
		
		/*!
			\brief Complements the data.
			\detail The complement is a complement on base.
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

		bool is_signed = false; /*! < Is true if the represented number is signed, else false. */
		constexpr static base_type base = static_cast<base_type>(std::numeric_limits<data_type>::max()) + 1; /*! the base of the number system */
		constexpr static data_type base_min_one = std::numeric_limits<data_type>::max(); /*! the base-1 of the number system */

	public:
		using size_type = decltype(data.size()); /*! the size type used by this class */
		bigint(data_type n);
		bigint(std::int32_t n);
		bigint(std::int64_t n);
		bigint(std::string s);
		bigint(const bigint& other) : data(other.data), is_signed(other.is_signed) {}
		void operator++() noexcept;
		bigint operator+(const bigint& other) noexcept;
		bigint operator-(const bigint& other) noexcept;
		bigint operator*(const bigint& other) noexcept;
		bigint operator/(const bigint& other) noexcept;
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

	std::ostream& operator<<(std::ostream& out, const bigint& b)
	{
		return out << b.to_string();
	}

	template class bigint<uint8_t>;
	template class bigint<uint16_t>;
	template class bigint<uint32_t>;
#ifdef GNUC
	template class bigint<uint64_t>; /*! < only supported when using gcc or clang*/
#endif // GNUC

} // namespace numeric
