//(C) 2018 fahaas
/*!
 *	\file bigint.cpp
 *	\brief implementation of bigint class
 */
#include "bigint.hpp"
#include "algorithms.hpp"
#include <algorithm>
#include <array>
#include <limits>

constexpr std::array<uint64_t, 16> p16 = {1,
                                          16,
                                          256,
                                          4096,
                                          65536,
                                          1048576,
                                          16777216,
                                          268435456,
                                          4294967296,
                                          68719476736,
                                          1099511627776,
                                          17592186044416,
                                          281474976710656,
                                          4503599627370496,
                                          72057594037927936,
                                          1152921504606846976};

namespace numeric {
    template <typename data_type, class vector_allocator>
    bigint<data_type, vector_allocator>::bigint(data_type n) noexcept
	{
        data.push_back(n);
    }

    template <typename data_type, typename vector_allocator>
    bigint<data_type, vector_allocator>::bigint(std::int64_t n) noexcept
	{
        auto is_min = false;

        if (n == 0) {
            data.push_back(0);
            is_signed = false;
            return;
        }

        if (n < 0) {
            // an input value of -2^63 would lead to unexpected behaviour because abs(std::int64_t::max-1) ==
            // abs(std::int64_t::min)
            if (n == std::numeric_limits<std::int64_t>::min()) {
                ++n;
                is_min = true;
            }
            n *= -1;
            is_signed = true;
        }

        // simple algorithm to convert to base
        while (n != 0) {
            data.push_back(static_cast<data_type>(n % base));
            n -= n % base;
            n /= base;
        }

        /*if (is_min)
            this->operator--();*/
    }

    template <typename data_type, typename vector_allocator>
    bigint<data_type, vector_allocator>::bigint(std::string hex_str)
    {
        std::reverse(hex_str.begin(), hex_str.end());
        if (hex_str.back() == '-') {
            is_signed = true;
            hex_str.pop_back();
        }

        std::for_each(hex_str.begin(), hex_str.end(), [](auto& c) {
            if (c >= 48 && c <= 57)
                c -= 48;
            else if (c >= 65 && c <= 90)
                c -= 55;
            else if (c >= 97 && c <= 122)
                c -= 87;
            else
                throw std::invalid_argument("Input string not valid!");
        });

        constexpr std::uint8_t block_size = sizeof(data_type) * 2;
        data.resize(hex_str.size() / block_size + (hex_str.size() % block_size == 0 ? 0 : 1), 0);

        std::uint8_t block_i = 0;
        std::size_t this_data_i = 0;
        for (char i : hex_str) {
            data[this_data_i] += i * p16[block_i];
            ++block_i;

            if (block_i == block_size) {
                ++this_data_i;
                block_i = 0;
            }
        }
    }

	template<typename data_type, class vector_allocator>
	bigint<data_type, vector_allocator>& bigint<data_type, vector_allocator>::operator*=(bigint other) noexcept
	{
		std::vector<data_container_type> results(other.data.size());

		#pragma omp parallel for
		for (size_type i = 0; i < other.data.size(); ++i)
			results[i] = rshift<data_type>(mult_with_digit(data, other.data[i]), i);

		sum_vec(results, data);

		is_signed = is_signed != other.is_signed;

		return *this;
	}
} // namespace numeric
