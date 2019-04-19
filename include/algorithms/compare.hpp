// ---------- (C) 2018, 2019 fahaas ----------
#pragma once

#include <cstddef>

namespace xenonis::algorithms {
    template <class InContainer>
    bool greater(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer> bool less(const InContainer& a, const InContainer& b, bool or_equal = false) noexcept;

    template <class InContainer> bool is_zero(InContainer first, InContainer last) noexcept;

    template <class InContainer> bool greater(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() > b.size();

        for (std::size_t i = a.size() - 1; i < a.size(); --i) {
            if (a[i] != b[i])
                return a[i] > b[i];
        }
        return or_equal;
    }

    template <class InContainer> bool less(const InContainer& a, const InContainer& b, bool or_equal) noexcept
    {
        if (a.size() != b.size())
            return a.size() < b.size();

        for (std::size_t i = a.size() - 1; i < a.size(); --i) {
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
