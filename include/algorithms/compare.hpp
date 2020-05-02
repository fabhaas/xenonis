//******************************************************************************
//* Copyright 2018-2020 Fabian Haas                                            *
//*                                                                            *
//* This Source Code Form is subject to the terms of the Mozilla Public        *
//* License, v. 2.0. If a copy of the MPL was not distributed with this        *
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.                  *
//******************************************************************************

#pragma once

#include <cstddef>
#include <iterator>

namespace xenonis::algorithms {
    template <class InIter>
    bool greater(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal, std::size_t a_size,
                 std::size_t b_size) noexcept;

    template <class InIter>
    bool greater(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal) noexcept;

    template <class InIter>
    bool less(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal, std::size_t a_size,
              std::size_t b_size) noexcept;

    template <class InIter>
    bool less(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal) noexcept;

    template <class InContainer>
    bool greater(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer> bool less(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer> bool is_zero(InContainer first, InContainer last) noexcept;

    template <class InIter>
    bool greater(InIter a_first, InIter a_last, InIter b_last, bool or_equal, std::size_t a_size,
                 std::size_t b_size) noexcept
    {
        if (a_size != b_size)
            return a_size > b_size;

        auto ra_first{std::make_reverse_iterator(a_last)};
        auto ra_last{std::make_reverse_iterator(a_first)};
        auto rb_first{std::make_reverse_iterator(b_last)};
        for (; ra_first != ra_last; ++ra_first, ++rb_first) {
            if (*ra_first != *rb_first)
                return *ra_first > *rb_first;
        }
        return or_equal;
    }

    template <class InIter>
    bool greater(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal) noexcept
    {
        return greater(a_first, a_last, b_last, or_equal, std::distance(a_first, a_last),
                       std::distance(b_first, b_last));
    }

    template <class InIter>
    bool less(InIter a_first, InIter a_last, InIter b_last, bool or_equal, std::size_t a_size,
              std::size_t b_size) noexcept
    {
        if (a_size != b_size)
            return a_size < b_size;

        auto ra_first{std::make_reverse_iterator(a_last)};
        auto ra_last{std::make_reverse_iterator(a_first)};
        auto rb_first{std::make_reverse_iterator(b_last)};
        for (; ra_first != ra_last; ++ra_first, ++rb_first) {
            if (*ra_first != *rb_first)
                return *ra_first < *rb_first;
        }
        return or_equal;
    }

    template <class InIter>
    bool less(InIter a_first, InIter a_last, InIter b_first, InIter b_last, bool or_equal) noexcept
    {
        return less(a_first, a_last, b_last, or_equal, std::distance(a_first, a_last), std::distance(b_first, b_last));
    }

    template <class InContainer> bool greater(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() > b.size();

        for (std::size_t i{a.size() - 1}; i < a.size(); --i) {
            if (a[i] != b[i])
                return a[i] > b[i];
        }
        return or_equal;
    }

    template <class InContainer> bool less(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() < b.size();

        for (std::size_t i{a.size() - 1}; i < a.size(); --i) {
            if (a[i] != b[i])
                return a[i] < b[i];
        }

        return or_equal; // a == b
    }

    template <class InContainer> bool is_zero(InContainer first, InContainer last) noexcept
    {
        for (; first != last; ++first)
            if (*first != 0)
                return false;

        return true;
    }
} // namespace xenonis::algorithms
