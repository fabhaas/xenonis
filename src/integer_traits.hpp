// ---------- (C) 2018 fahaas ----------
#pragma once

#include <cstdint>
#include <type_traits>

namespace xenonis::traits {

#ifdef XENONIS_USE_UINT128
    using uint128_t = unsigned __int128;
#endif

    template <typename T> struct get_doubled {
        static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value, "Not supported");
        using type = typename std::conditional<
            std::is_same<T, std::uint8_t>::value, std::uint16_t,
            typename std::conditional<
                std::is_same<T, std::uint16_t>::value, std::uint32_t,
                typename std::conditional<
                    std::is_same<T, std::uint32_t>::value, std::uint64_t,
                    typename std::conditional<std::is_same<T, std::uint64_t>::value,
#ifdef XENONIS_USE_UINT128
                                              uint128_t
#else
                                              void
#error Not supported when not using GCC or Clang
#endif
                                              ,
                                              void>::type>::type>::type>::type;
    };
} // namespace xenonis::traits
