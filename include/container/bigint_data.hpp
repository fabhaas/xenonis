// ---------- (C) 2018, 2019 fahaas ----------
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "data_view.hpp"

namespace xenonis::internal {
    template <typename Value, typename Size> class data_view; //implemented in bigint_data_view.hpp
    template <typename Value, class Allocator = std::allocator<Value>> class bigint_data {
        using size_type = std::size_t;
        friend class data_view<Value, size_type>;
        //Allocator m_alloc;
        size_type m_size;
        size_type m_capacity;
        Value* m_ptr = nullptr;
        auto allocate(size_type size)
        {
            //return m_alloc.allocate(size);
            return static_cast<Value*>(calloc(size, sizeof(Value)));
        }
        void deallocate()
        {
            free(m_ptr);
            //m_alloc.deallocate(m_ptr, m_capacity);
        }

      public:
        bigint_data() : m_size(0), m_capacity(0), m_ptr(nullptr) {}

        bigint_data(size_type n) : m_size(n), m_capacity(n), m_ptr(allocate(n)) {}

        bigint_data(size_type n, Value val) : bigint_data(n) { std::fill(begin(), end(), val); }

        bigint_data(const bigint_data& other)
            : m_size(other.m_size), m_capacity(other.m_capacity),
              m_ptr(allocate(other.m_capacity))
        {
            std::copy(other.cbegin(), other.cend(), begin());
        }

        bigint_data(bigint_data&& other)
            : m_size(other.m_size), m_capacity(other.m_capacity), m_ptr(other.m_ptr)
        {
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_ptr = nullptr;
        }

        bigint_data& operator=(const bigint_data& other)
        {
            if (m_ptr != nullptr)
                deallocate();

            m_ptr = allocate(m_capacity);
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            std::copy(other.cbegin(), other.cend(), begin());

            return *this;
        }

        bigint_data& operator=(bigint_data&& other)
        {
            if (m_ptr != nullptr)
                deallocate();

            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;

            return *this;
        }

        void push_back(Value val)
        {
            if (m_capacity > m_size) {
                ++m_size;
                m_ptr[m_size - 1] = val;
            } else {
                auto* tmp = allocate(m_size + 1);
                std::copy(begin(), end(), tmp);
                deallocate();

                m_ptr = tmp;
                ++m_size;
                m_capacity = m_size;
            }
        }

        inline void pop_back() noexcept { --m_size; }
        inline void pop_n(size_type n) noexcept { m_size -= n; }

        void resize(size_type new_size)
        {
            if (new_size < m_size) {
                m_size = new_size;
                return;
            }

            auto* tmp = allocate(new_size);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp);

            deallocate();

            m_size = new_size;
            m_capacity = new_size;
            m_ptr = tmp;
        }

        void resize(size_type new_size, Value val)
        {
            if (new_size < m_size) {
                m_size = new_size;
                return;
            }

            auto* tmp = allocate(new_size);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp);

            for (size_type i = m_size; i < new_size; ++i)
                tmp[i] = val;

            deallocate();

            m_size = new_size;
            m_capacity = new_size;
            m_ptr = tmp;
        }

        inline Value& operator[](size_type i) noexcept { return m_ptr[i]; }
        inline const Value& operator[](size_type i) const noexcept { return m_ptr[i]; }

        inline Value& front() noexcept { return m_ptr[0]; }
        inline const Value& front() const noexcept { return m_ptr[0]; }
        inline Value& back() noexcept { return m_ptr[m_size - 1]; }
        inline const Value& back() const noexcept { return m_ptr[m_size - 1]; }

        inline Value* data() const noexcept { return m_ptr; }
        inline auto begin() noexcept { return m_ptr; }
        inline auto end() noexcept { return m_ptr + m_size; }
        inline const Value* begin() const noexcept { return m_ptr; }
        inline const Value* end() const noexcept { return m_ptr + m_size; }
        inline const Value* cbegin() const noexcept { return m_ptr; }
        inline const Value* cend() const noexcept { return m_ptr + m_size; }
        inline auto rbegin() noexcept { return std::make_reverse_iterator(end()); }
        inline auto rend() noexcept { return std::make_reverse_iterator(begin()); }
        inline auto crbegin() const noexcept { return std::make_reverse_iterator(cend()); }
        inline auto crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

        bool operator==(const bigint_data& other) const noexcept
        {
            if (m_size != other.m_size)
                return false;

            return std::equal(other.begin(), other.end(), begin());
        }
        bool operator!=(const bigint_data& other) const noexcept { return !operator==(other); }

        inline bool empty() const noexcept { return m_size == 0; }
        inline auto size() const noexcept { return m_size; }
        inline auto capacity() const noexcept { return m_capacity; }

        ~bigint_data()
        {
            if (m_ptr != nullptr)
                deallocate();
        }
    };
} // namespace xenonis::internal
