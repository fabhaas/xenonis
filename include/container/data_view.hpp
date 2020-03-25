// ---------- (C) 2018-2020 Fabian Haas ----------
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include "bigint_data.hpp"

namespace xenonis::internal {
    template <typename Value, typename Size> class data_view {
      private:
        Value* m_ptr;
        Size m_size;

      public:
        data_view() : m_ptr(nullptr), m_size(0) {}
        data_view(Value* m_ptr, Size size_) : m_ptr(m_ptr), m_size(size_)
        {
            assert(m_ptr != NULL);
            assert(m_ptr != nullptr);
        }

        data_view(const data_view& other) : m_ptr(other.m_ptr), m_size(other.m_size) {}
        inline const Value& operator[](Size i) const noexcept { return m_ptr[i]; }

        inline const Value& front() const noexcept { return m_ptr[0]; }
        inline Value& front() noexcept { return m_ptr[0]; }
        inline const Value& back() const noexcept { return m_ptr[m_size - 1]; }
        inline Value& back() noexcept { return m_ptr[m_size - 1]; }

        inline const Value* data() const noexcept { return m_ptr; }
        inline const Value* begin() const noexcept { return m_ptr; }
        inline const Value* end() const noexcept { return m_ptr + m_size; }
        inline const Value* cbegin() const noexcept { return m_ptr; }
        inline const Value* cend() const noexcept { return m_ptr + m_size; }
        inline auto crbegin() const noexcept { return std::make_reverse_iterator(cend()); }
        inline auto crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

        void resize(Size new_size)
        {
            if (new_size > m_size)
                throw std::logic_error("Cannot expand view");
            else
                m_size = new_size;
        }

        bool operator==(const data_view& other) const noexcept
        {
            if (m_size != other.m_size)
                return false;

            return std::equal(other.begin(), other.end(), begin());
        }
        bool operator!=(const data_view& other) const noexcept { return !operator==(other); }

        inline bool empty() const noexcept { return m_size == 0; }
        inline auto size() const noexcept { return m_size; }
    };

    template <class Iter>
    auto make_view(Iter first, Iter last)
    {
        return data_view<std::remove_reference_t<decltype(*first)>, std::size_t>(first, std::distance(first, last));
    }
} // namespace xenonis::internal
