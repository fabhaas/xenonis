// ---------- (C) 2018-2020 Fabian Haas ----------
#pragma once

#include <algorithm>

namespace xenonis::algorithms {
    /*!
     * \brief Appends count zeros to the front. e.g data = { 1, 2, 3 }, count = 3 -> { 0, 0, 0, 1,
     * 2, 3 }
     */
    template <class InContainer, class OutContainer = InContainer>
    OutContainer lshift(const InContainer& data, decltype(data.size()) count);

    /*!
     * \brief Removes all redundant zeros. z.B { 0, 1, 2 } -> { 1, 2 }
     * \details Never returns empty container. z.B { 0, 0, 0 } -> { 0 }
     */
    template <class InContainer> void remove_zeros(InContainer& data) noexcept;

    template <class InContainer, class OutContainer>
    OutContainer lshift(const InContainer& data, decltype(data.size()) count)
    {
        OutContainer tmp(data.size() + count, 0);
        std::copy(data.cbegin(), data.cend(), tmp.begin() + count);
        return tmp;
    }

    template <class InContainer> void remove_zeros(InContainer& data) noexcept
    {
        auto first{data.crbegin()};
        auto last{data.crend()};

        for (; first != last; ++first)
            if (*first != 0)
                break;

        if (first == last) { // data.size() must not be 0
            data.resize(1);
            data.front() = 0;
            return;
        }

        data.resize(last - first);
    }
} // namespace xenonis::algorithms
