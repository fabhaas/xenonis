// ---------- (C) 2018-2020 Fabian Haas ----------
#pragma once

#include "bigint_config.hpp"

#include <cstdint>
#include <type_traits>

namespace xenonis::traits {
    template <typename T> struct uinteger {
        static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value, "T has to be an unsigned integer");
        using doubled = std::conditional_t<
            std::is_same_v<T, std::uint8_t>, std::uint16_t,
            std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint32_t,
                               std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint64_t,
                                                  std::conditional_t<std::is_same_v<T, std::uint64_t>,
#ifdef XENONIS_USE_UINT128
                                                                     uint128_t
#else
                                                                     void
#endif
                                                                     ,
                                                                     void>>>>;
        using halved =
#ifdef XENONIS_USE_UINT128
            std::conditional_t<
                std::is_same_v<T, uint128_t>, std::uint64_t,
#endif
                std::conditional_t<std::is_same_v<T, std::uint64_t>, std::uint32_t,
                                   std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint8_t, void>>
#ifdef XENONIS_USE_UINT128
                >
#endif
            ;
    };
    template <typename T> struct integer {
        static_assert(std::is_signed<T>::value && std::is_integral<T>::value, "T has to be a signed integer");

        using unsigned_type = std::conditional_t<
            std::is_same_v<std::int64_t, T>, std::uint64_t,
            std::conditional_t<
                std::is_same_v<std::int32_t, T>, std::uint32_t,
                std::conditional_t<std::is_same_v<std::int16_t, T>, std::uint16_t,
                                   std::conditional_t<std::is_same_v<std::int8_t, T>, std::uint8_t, void>>>>;
    };
} // namespace xenonis::traits
