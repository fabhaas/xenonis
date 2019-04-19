// ---------- (C) 2018, 2019 fahaas ----------
#pragma once

#include "util.hpp"
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string_view>
#include <string>
#include <stdexcept>

namespace xenonis::algorithms {
    template <typename Value, class InContainer>
    std::string hex_to_string(const InContainer& a, bool is_signed, bool lower_case = true);

    template <typename Value, class OutContainer> OutContainer hex_from_string(const std::string_view str);

    template <typename Value, class InContainer>
    std::string hex_to_string(const InContainer& data, bool is_signed, bool lower_case)
    {
        using size_type = decltype(data.size());
        std::string ret(data.size() * sizeof(Value) * 2, '\0');

        // convert to hex
        constexpr std::uint8_t mask0 = 0xF0;
        constexpr std::uint8_t mask1 = 0xF;

        size_type i_ret = 0;
        std::for_each(data.cbegin(), data.cend(), [&ret, &i_ret](Value n) {
            auto* n_ptr = reinterpret_cast<std::uint8_t*>(&n);

            for (size_type i = 0; i < sizeof(Value); ++i) {
                ret[i_ret++] = static_cast<char>(mask1 & n_ptr[i]);
                ret[i_ret++] = static_cast<char>((mask0 & n_ptr[i]) >> 4);
            }
        });

        // remove zeros
        remove_zeros(ret);

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

    template <typename Value, class OutContainer> OutContainer hex_from_string(const std::string_view str)
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

        constexpr std::uint8_t block_size = sizeof(Value) * 2;

        OutContainer ret(str.size() / block_size + (str.size() % block_size != 0), 0);

        auto ret_first = ret.begin();
        std::uint8_t i_block = 0;
        auto str_first = str.crbegin();
        auto str_last = str.crend();
        for (; str_first != str_last; ++str_first) {
            *ret_first |= (static_cast<Value>(conv(*str_first)) << (4 * (i_block++)));

            if (i_block == block_size) {
                i_block = 0;
                ++ret_first;
            }
        }

        return ret;
    }
} // namespace xenonis::algorithms
