#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>

namespace xenonis::internal {
    template <typename data_type, typename allocater_type = std::allocator<data_type>>
    class bigint_data {
      public:
        using size_type = std::size_t;

      private:
        size_type size;
        allocater_type allocator;
        data_type* data;

      public:
        bigint_data(size_type size, std::optional<data_type> val = std::nullopt) : size(size)
        {
            data = new data_type[size];

            if (val.has_value())
                std::fill(data, data + size, val);
        }

        bigint_data(const bigint_data& other) : size(other.size)
        {
            data = new data_type[other.size];
            std::copy(other.data, other.data + size, data);
        }

        bigint_data(bigint_data&& other) : size(other.size), data(other.data)
        {
            // other.size = 0;
            other.data = nullptr;
        }

        const data_type& operator[](size_type n) const { return data[n]; }

        data_type& operator[](size_type n) { return data[n]; }

        bigint_data_iter<data_type, size_type> begin()
        {
            return bigint_data_iter<data_type, size_type>(data, size);
        }

        bigint_data_iter<data_type, size_type> end()
        {
            return bigint_data_iter<data_type, size_type>(data, size, size - 1);
        }
    };
} // namespace xenonis::internal
