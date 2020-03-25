// ---------- (C) 2018-2020 Fabian Haas ----------
#pragma once

#include "../integer_traits.hpp"
#include "util.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace xenonis::algorithms {
    template <typename Value, class InContainer>
    std::string to_string(const InContainer& a, bool is_signed, bool lower_case = true);

    template <typename Value, class OutContainer> OutContainer from_string(const std::string_view str);

    template <typename Value, class InContainer>
    std::string to_string(const InContainer& data, bool is_signed, bool lower_case)
    {
        using size_type = decltype(data.size());
        std::string ret(data.size() * sizeof(Value) * 2, '\0');

        // convert to hex
        constexpr std::uint8_t mask0{0xF0};
        constexpr std::uint8_t mask1{0xF};

        size_type i_ret{0};
        std::for_each(data.cbegin(), data.cend(), [&ret, &i_ret](Value n) {
            auto* n_ptr{reinterpret_cast<std::uint8_t*>(&n)};

            for (size_type i{0}; i < sizeof(Value); ++i) {
                ret[i_ret++] = static_cast<char>(mask1 & n_ptr[i]);
                ret[i_ret++] = static_cast<char>((mask0 & n_ptr[i]) >> 4);
            }
        });

        // remove zeros
        remove_zeros(ret);

        // convert to string
        const auto letter_dis{static_cast<char>(lower_case ? 87 : 55)};
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

    template <typename Value, class OutContainer> OutContainer from_string(const std::string_view str)
    {
        auto conv = [](auto c) {
            if (c >= 48 && c <= 57)
                return c - 48;
            else if (c >= 65 && c <= 90)
                return c - 55;
            else if (c >= 97 && c <= 122)
                return c - 87;
            else
                throw std::invalid_argument("Input string not valid!: invalid char");
        };

        constexpr std::uint8_t block_size{sizeof(Value) * 2};

        OutContainer ret(str.size() / block_size + (str.size() % block_size != 0), 0);

        auto ret_first{ret.begin()};
        std::uint8_t i_block{0};
        auto str_first{str.crbegin()};
        auto str_last{str.crend()};
        for (; str_first != str_last; ++str_first) {
            *ret_first |= (static_cast<Value>(conv(*str_first)) << (4 * (i_block++)));

            if (i_block == block_size) {
                i_block = 0;
                ++ret_first;
            }
        }

        return ret;
    }

    template <typename OutValue, typename InValue, class OutContainer> OutContainer from_uint(InValue n)
    {
        if constexpr (sizeof(OutValue) >= sizeof(InValue)) {
            return OutContainer(1, n);
        } else {
            static_assert(sizeof(InValue) % sizeof(OutValue) == 0, "Not supported!");
            OutContainer out(sizeof(InValue) / sizeof(OutValue), 0);
            auto mask{static_cast<InValue>(std::numeric_limits<OutValue>::max())};

            for (std::size_t i{0}; i < out.size(); ++i) {
                out[i] = (n & mask) >> (i * 8);
                mask <<= sizeof(OutValue) * 8;
            }

            return out;
        }
    }

    template <typename OutValue, typename InValue, class OutContainer> std::pair<bool, OutContainer> from_int(InValue n)
    {
        using unsigned_type = typename traits::integer<InValue>::unsigned_type;
        if (n < 0) {
            if (std::numeric_limits<InValue>::min() == n) {
                ++n;
                return std::make_pair(
                    true, from_uint<OutValue, unsigned_type, OutContainer>(static_cast<unsigned_type>(n * -1) + 1));
            } else {
                return std::make_pair(
                    true, from_uint<OutValue, unsigned_type, OutContainer>(static_cast<unsigned_type>(n * -1)));
            }
        } else {
            return std::make_pair(false,
                                  from_uint<OutValue, unsigned_type, OutContainer>(static_cast<unsigned_type>(n)));
        }
    }
} // namespace xenonis::algorithms
