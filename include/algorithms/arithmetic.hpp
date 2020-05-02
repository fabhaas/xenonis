//******************************************************************************
//* Copyright 2018-2020 Fabian Haas                                            *
//*                                                                            *
//* This Source Code Form is subject to the terms of the Mozilla Public        *
//* License, v. 2.0. If a copy of the MPL was not distributed with this        *
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.                  *
//******************************************************************************

/*!
 *  \file arithmetic.hpp
 *  Implements the arithmetic algorithms used in bigint.hpp
 */
#pragma once

#include "../integer_traits.hpp"
#include "compare.hpp"
#include "util.hpp"
//#include <tbb/task_group.h> // Intel Thread Building Blocks library for parallelization, may be used in the future
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <type_traits>

namespace xenonis::algorithms {
    /*!
     *  Adds b to a and writes the result to c. Requires a.size() >= b.size() and c.size() >= a.size(). It is possible
     *  that a is c (then b will be added to a).
     *  \param a_first iterator poiting to first element of a. Could be const iterator. Could be equal c.
     *  \param b_first iterator pointing to first element of b. Could be const iterator. Must not be equal c.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \param c_first iterator pointing to first element of c. Could be equal a.
     *  \returns carry
     */
    template <class InIter, class OutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        inline bool
        add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

    /*!
     *  Subtracts b from a and writes the result to c. Requires a.size() >= b.size() and c.size() >= a.size(). c must
     *  not be a.
     *  \param a_first iterator poiting to first element of a. Could be const iterator.
     *  \param b_first iterator pointing to first element of b. Could be const iterator.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \param c_first iterator pointing to first element of c.
     *  \returns carry
     */
    template <class InIter, class OutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        inline bool
        sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

    /*!
     *  Subtracts b from a and writes the result to a. Requires a.size() >= b.size().
     *  \param a_first iterator poiting to first element of a.
     *  \param b_first iterator pointing to first element of b. Could be const iterator.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \returns carry
     */
    template <class InIter, class InOutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        inline bool
        sub_from(InOutIter a_first, InIter b_first, InIter b_last);

    /*!
     *  Increments a by 1.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a
     *  \returns carry
     */
    template <class InIter> constexpr inline bool increment(InIter a_first, InIter a_last);

    /*!
     *  Decrements a by 1.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \returns carry
     */
    template <class InIter> constexpr inline bool decrement(InIter a_first, InIter a_last);

    /*!
     *  Multiplies a with b and returns the result.
     *  \returns the result
     */
    template <typename Value> constexpr inline std::array<Value, 2> base_mul(Value a, Value b);

    /*!
     *  Multiplies a with b and returns the result.
     *  \details Uses the naive method to multiply. Complexity: O(n^2)
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \returns the result
     */
    template <class OutContainer, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        OutContainer
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last);

    /*!
     *  Multiplies a with b and writes the result to out.
     *  \details Uses the naive method to multiply. Complexity: O(n^2)
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \param out_first iterator pointing to the first element of out.
     *  \param out_last iterator pointing to the last element of out.
     *  \param the size of a
     */
    template <class OutIter, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        void
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last, OutIter out_first);

    /*!
     *  Multiplies a with b and returns the result.
     *  \details Uses the Karatsuba Algorithm to multiply which is a recursive algorithm with a complexity of
     *  O(n^log2(3)). See e.g https://en.wikipedia.org/wiki/Karatsuba_algorithm.
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \returns the result
     */
    template <class OutContainer, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        OutContainer
        karatsuba_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer div(const InContainer& a, const InContainer& b);

    template <class InIter, class OutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        bool inline add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
#if defined(XENONIS_INLINE_ASM_AMD64)
        if constexpr (std::is_same_v<value_type, std::uint64_t>) {
            std::uint64_t carry{0}, size{static_cast<std::uint64_t>(std::distance(b_first, b_last))};
            // asm(R"(
            // xor %%rsi, %%rsi
            // movq %[max], %%rbx
            // subq %[size], %%rbx
            // addq $1, %%rbx
            //%=1:
            // movq (%[a], %%rsi, 8), %%rax
            // adcq (%[b], %%rsi, 8), %%rax
            // movq %%rax, (%[c], %%rsi, 8)
            // inc %%rsi
            // inc %%rbx
            // jno %=1b
            // adcq $0, %[carry]
            //)"
            //: [ c ] "+r"(c_first), [ carry ] "+r"(carry) // DO NOT USE = constraint when using pointer
            //: [ a ] "r"(a_first), [ b ] "r"(b_first), [ size ] "r"(size),
            //[ max ] "r"(std::numeric_limits<std::int64_t>::max())
            //: "rax", "rbx", "rcx", "rsi", "memory");

            // same in Intel syntax:
            // asm (R"(
            // xor rsi, rsi
            // mov rbx, %[max]
            // sub rbx, %[size]
            // add rbx, 1
            //%=1:
            // mov rax, QWORD PTR [%[a]+rsi*8]
            // adc rax, QWORD PTR [%[b]+rsi*8]
            // mov QWORD PTR [%[c]+rsi*8], rax
            // inc rsi
            // inc rbx
            // jno %=1b
            // adc %[carry], 0
            //)"
            //: [ c ] "+r"(c_first), [carry] "+r" (carry) // DO NOT USE = constraint when using pointer
            //: [ a ] "r" (a_first), [ b ] "r" (b_first), [ size ] "r" (size),  [ max ] "r"
            //(std::numeric_limits<std::int64_t>::max()) : "rax", "rbx", "rcx", "rsi", "memory");*/
            asm(R"(
                xor %%r8, %%r8
                movq $4, %%r9
                movq %[size], %%rax # rax = size
                cmp $4, %%rax
                jge %=1f
                xor %%rdx, %%rdx    # clear registers
                movq %%rax, %%rdx
                jmp %=3f
            %=1:
                xor %%rdx, %%rdx
                div %%r9
            %=2:
                movq (%[a], %%r8, 8), %%r10
                movq 8(%[a], %%r8, 8), %%r11
                movq 16(%[a], %%r8, 8), %%r12
                movq 24(%[a], %%r8, 8), %%r13
                adcx (%[b], %%r8, 8), %%r10
                adcx 8(%[b], %%r8, 8), %%r11
                adcx 16(%[b], %%r8, 8), %%r12
                adcx 24(%[b], %%r8, 8), %%r13
                movq %%r10, (%[c], %%r8, 8)
                movq %%r11, 8(%[c], %%r8, 8)
                movq %%r12, 16(%[c], %%r8, 8)
                movq %%r13, 24(%[c], %%r8, 8)
                adox %%r9, %%r8
                dec %%rax
                jnz %=2b

                inc %%rdx           # do not change the state of the CF
                dec %%rdx
                jz %=4f
            %=3:
                movq (%[a], %%r8, 8), %%r10
                adcq (%[b], %%r8, 8), %%r10
                movq %%r10, (%[c], %%r8, 8)
                inc %%r8
                dec %%rdx
                jnz %=3b
            %=4:
                adcq $0, %[carry]
            )"
                : [ c ] "+r"(c_first), [ carry ] "+r"(carry) // DO NOT USE = constraint when using pointer
                : [ a ] "r"(a_first), [ b ] "r"(b_first), [ size ] "rm"(size)
                : "rax", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "memory");
            return carry;
        } else {
#elif defined(__clang__) // for different architectures than x86
#if __has_builtin(__builtin_addcll)
        if constexpr (std::is_same_v<value_type, unsigned long long>) {
            unsigned long long carry_in{0}, carry_out{0};
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addcll(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#if __has_builtin(__builtin_addcl)
        if constexpr (std::is_same_v<value_type, unsigned long>) {
            unsigned long carry_in{0}, carry_out{0};
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addcl(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#if __has_builtin(__builtin_addc)
        if constexpr (std::is_same_v<value_type, unsigned int>) {
            unsigned int carry_in{0}, carry_out{0};
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addc(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#endif
            // portable
            bool carry{false};
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = *a_first + *b_first + carry;
                carry = carry ? *c_first <= *b_first : *c_first < *b_first;
            }
            return carry;
#if defined(XENONIS_INLINE_ASM_AMD64)
        }
#endif
    }

    template <class InIter, class OutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        inline bool
        sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
#if defined(XENONIS_INLINE_ASM_AMD64)
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;

        if constexpr (std::is_same_v<value_type, std::uint64_t>) {
            std::uint64_t carry{0};
            auto size{static_cast<std::uint64_t>(std::distance(b_first, b_last))};
            asm(R"(
            xor %%rsi, %%rsi
            movq %[max], %%rbx
            subq %[size], %%rbx
            addq $1, %%rbx
            %=1:
            movq (%[a], %%rsi, 8), %%rax
            sbbq (%[b], %%rsi, 8), %%rax
            movq %%rax, (%[c], %%rsi, 8)
            inc %%rsi
            inc %%rbx
            jno %=1b
            sbbq $0, %[carry]
            )"
                : [ c ] "+r"(c_first), [ carry ] "+r"(carry) // DO NOT USE = constraint when using pointer
                : [ a ] "r"(a_first), [ b ] "r"(b_first), [ size ] "r"(size),
                  [ max ] "r"(std::numeric_limits<std::int64_t>::max())
                : "rax", "rbx", "rcx", "rsi", "memory");
            return carry;
        } else {
#endif
            bool carry{false};
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = *a_first - *b_first - carry;
                carry = carry ? *c_first >= *a_first : *c_first > *a_first;
            }
            return carry;
#if defined(XENONIS_INLINE_ASM_AMD64)
        }
#endif
    }

    template <class InIter, class InOutIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        inline bool
        sub_from(InOutIter a_first, InIter b_first, InIter b_last)
    {
#if defined(XENONIS_INLINE_ASM_AMD64)
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;

        if constexpr (std::is_same_v<value_type, std::uint64_t>) {
            std::uint64_t carry{0}, size{static_cast<std::uint64_t>(std::distance(b_first, b_last))};
            asm(R"(
            xor %%rsi, %%rsi
            movq %[max], %%rbx
            subq %[size], %%rbx
            addq $1, %%rbx
            %=1:
            movq (%[a], %%rsi, 8), %%rax
            sbbq (%[b], %%rsi, 8), %%rax
            movq %%rax, (%[a], %%rsi, 8)
            inc %%rsi
            inc %%rbx
            jno %=1b
            sbbq $0, %[carry]
            )"
                : [ a ] "+r"(a_first), [ carry ] "+r"(carry) // DO NOT USE = constraint when using pointer
                : [ b ] "r"(b_first), [ size ] "r"(size), [ max ] "r"(std::numeric_limits<std::int64_t>::max())
                : "rax", "rbx", "rcx", "rsi", "memory");
            return carry;
        } else {
#endif
            bool carry{false};
            for (; b_first != b_last; ++a_first, ++b_first) {
                auto tmp = *a_first;
                *a_first -= *b_first + carry;
                carry = carry ? *a_first >= tmp : *a_first > tmp;
            }
            return carry;
#if defined(XENONIS_INLINE_ASM_AMD64)
        }
#endif
    }

    template <class InIter> constexpr inline bool increment(InIter a_first, InIter a_last)
    {
        for (; a_first != a_last; ++a_first)
            if (++(*a_first) != 0)
                return false;
        return true;
    }

    template <class InIter> constexpr inline bool decrement(InIter a_first, InIter a_last)
    {
        for (; a_first != a_last; ++a_first)
            if (--(*a_first) != std::numeric_limits<std::remove_reference_t<decltype(*a_first)>>::max())
                return false;
        return true;
    }

    template <typename Value> constexpr inline std::array<Value, 2> base_mul(Value a, Value b)
    {
        using doubled = typename traits::uinteger<Value>::doubled;
        using halved = typename traits::uinteger<Value>::halved;

        if constexpr (std::is_same_v<doubled, void>) {
            halved res[4]{{0}}; // do not forget to initialize with 0
            auto* a_halved{reinterpret_cast<halved*>(&a)};
            auto* b_halved{reinterpret_cast<halved*>(&b)};
            naive_mul(a_halved, a_halved + 2, b_halved, b_halved + 2, res, res + 4);

            std::array<Value, 2> ret{{0,0}};
            std::memcpy(ret.data(), res, sizeof(Value) * 2);
            return ret;
        } else {
            doubled res{static_cast<doubled>(a) * b};
            std::array<Value, 2> ret{{0,0}};
            std::memcpy(ret.data(), &res, sizeof(doubled));
            return ret;
        }
    }

    template <class OutIter, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        void
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last, OutIter out_first)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
#if defined(XENONIS_INLINE_ASM_AMD64)
        if constexpr (std::is_same_v<value_type, std::uint64_t>) {
            const auto a_size{std::distance(a_first, a_last)};

            if (a_size % 2 == 0) {
                for (; b_first != b_last; ++b_first, ++out_first) {
                    if (*b_first == 0)
                        continue;

                    asm(R"(
                                xor %%rax, %%rax
                                xor %%rcx, %%rcx
                                mov $1, %%rsi
                                mov %[digit], %%rdx

                                mulx (%[in]), %%r8, %%r9
                                adcx (%[out]), %%r8
                                mov %%r8, (%[out])

                                setc %%al
                                movzx %%al, %%rax
                                cmp %%rsi, %[size]
                                jng %=2f

                            %=1:
                                mulx (%[in],%%rsi,8), %%r10, %%r11
                                adcx %%rax, %%r9
                                adcx (%[out],%%rsi,8), %%r9
                                adox %%rcx, %%r9
                                adox %%r10, %%r9
                                mov %%r9, (%[out],%%rsi,8)

                                mulx 8(%[in],%%rsi,8), %%r8, %%r9
                                adcx 8(%[out],%%rsi,8), %%r11
                                adox %%r8, %%r11
                                mov %%r11, 8(%[out],%%rsi,8)

                                setc %%al
                                movzx %%al, %%rax
                                seto %%cl
                                movzx %%cl, %%rcx
                                add $2, %%rsi
                                cmp %%rsi, %[size]
                                jg %=1b

                            %=2:
                                mulx (%[in],%%rsi,8), %%r10, %%r11
                                adcx %%rax, %%r9
                                adcx (%[out],%%rsi,8), %%r9
                                adox %%rcx, %%r9
                                adox %%r10, %%r9
                                mov %%r9, (%[out],%%rsi,8)

                                mov $0, %%rax
                                adcx 8(%[out],%%rsi,8), %%r11
                                adox %%rax, %%r11
                                mov %%r11, 8(%[out],%%rsi,8)
                        )"
                        : [ out ] "+r"(out_first)
                        : [ in ] "r"(a_first), [ digit ] "rm"(*b_first), [ size ] "r"(a_size - 1)
                        : "rax", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory");

                    // same in Intel syntax:
                    // asm (R"(
                    // xor rax, rax
                    // xor rcx, rcx
                    // mov rsi, 1
                    // mov rdx, %[digit]
                    //
                    // mulx r9, r8, QWORD PTR [%[in]]
                    // adcx r8, QWORD PTR [%[out]]
                    // mov QWORD PTR [%[out]], r8
                    //
                    // setc al
                    // movzx rax, al
                    // cmp %[size], rsi
                    // jng %=2f
                    //
                    //%=1:
                    // mulx r11, r10, QWORD PTR [%[in]+rsi*8]
                    // adcx r9, rax
                    // adcx r9, QWORD PTR [%[out]+rsi*8]
                    // adox r9, rcx
                    // adox r9, r10
                    // mov QWORD PTR [%[out]+rsi*8], r9
                    //
                    // mulx r9, r8, QWORD PTR 8[%[in]+rsi*8]
                    // adcx r11, QWORD PTR 8[%[out]+rsi*8]
                    // adox r11, r8
                    // mov QWORD PTR 8[%[out]+rsi*8], r11
                    //
                    // setc al
                    // movzx rax, al
                    // seto cl
                    // movzx rcx, cl
                    // add rsi, 2
                    // cmp %[size], rsi
                    // jg %=1b
                    //
                    //%=2:
                    // mulx r11, r10, QWORD PTR [%[in]+rsi*8]
                    // adcx r9, rax
                    // adcx r9, QWORD PTR [%[out]+rsi*8]
                    // adox r9, rcx
                    // adox r9, r10
                    // mov QWORD PTR [%[out]+rsi*8], r9
                    //
                    // mov rax, 0
                    // adcx r11, QWORD PTR 8[%[out]+rsi*8]
                    // adox r11, rax
                    // mov QWORD PTR 8[%[out]+rsi*8], r11
                    //)"
                    //: [out] "+r" (out_first)
                    //: [in] "r" (a_first), [digit] "rm" (digit), [size] "r" (a_size - 1)
                    //: "rax", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory"
                    //);
                }
            } else {
                for (; b_first != b_last; ++b_first, ++out_first) {
                    if (*b_first == 0)
                        continue;

                    asm(R"(
                                xor %%rax, %%rax
                                xor %%rcx, %%rcx
                                mov $1, %%rsi
                                mov %[digit], %%rdx

                                mulx (%[in]), %%r8, %%r9
                                adcx (%[out]), %%r8
                                mov %%r8, (%[out])

                                setc %%al
                                movzx %%al, %%rax
                                cmp %%rsi, %[size]
                                jng %=2f

                            %=1:
                                mulx (%[in],%%rsi,8), %%r10, %%r11
                                adcx %%rax, %%r9
                                adcx (%[out],%%rsi,8), %%r9
                                adox %%rcx, %%r9
                                adox %%r10, %%r9
                                mov %%r9, (%[out],%%rsi,8)

                                mulx 8(%[in],%%rsi,8), %%r8, %%r9
                                adcx 8(%[out],%%rsi,8), %%r11
                                adox %%r8, %%r11
                                mov %%r11, 8(%[out],%%rsi,8)

                                setc %%al
                                movzx %%al, %%rax
                                seto %%cl
                                movzx %%cl, %%rcx
                                add $2, %%rsi
                                cmp %%rsi, %[size]
                                jg %=1b

                            %=2:
                                adcx (%[out],%%rsi,8), %%r9
                                adcx %%rax, %%r9
                                adox %%rcx, %%r9
                                mov %%r9, (%[out],%%rsi,8)
                        )"
                        : [ out ] "+r"(out_first)
                        : [ in ] "r"(a_first), [ digit ] "rm"(*b_first), [ size ] "r"(a_size)
                        : "rax", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory");

                    // same in Intel syntax:
                    // asm (R"(
                    // xor rax, rax
                    // xor rcx, rcx
                    // mov rsi, 1
                    // mov rdx, %[digit]
                    //
                    // mulx r9, r8, QWORD PTR [%[in]]
                    // adcx r8, QWORD PTR [%[out]]
                    // mov QWORD PTR [%[out]], r8
                    //
                    // setc al
                    // movzx rax, al
                    // cmp %[size], rsi
                    // jng %=2f
                    //
                    //%=1:
                    // mulx r11, r10, QWORD PTR [%[in]+rsi*8]
                    // adcx r9, rax
                    // adcx r9, QWORD PTR [%[out]+rsi*8]
                    // adox r9, rcx
                    // adox r9, r10
                    // mov QWORD PTR [%[out]+rsi*8], r9
                    //
                    // mulx r9, r8, QWORD PTR 8[%[in]+rsi*8]
                    // adcx r11, QWORD PTR 8[%[out]+rsi*8]
                    // adox r11, r8
                    // mov QWORD PTR 8[%[out]+rsi*8], r11
                    //
                    // setc al
                    // movzx rax, al
                    // seto cl
                    // movzx rcx, cl
                    // add rsi, 2
                    // cmp %[size], rsi
                    // jg %=1b
                    //
                    //%=2:
                    // adcx r9, QWORD PTR [%[out]+rsi*8]
                    // adcx r9, rax
                    // adox r9, rcx
                    // mov QWORD PTR [%[out]+rsi*8], r9
                    //)"
                    //: [out] "+r" (out_first)
                    //: [in] "r" (a_first), [digit] "rm" (digit), [size] "r" (a_size)
                    //: "rax", "rcx", "rdx", "rbx", "rsi", "r8", "r9", "r10", "r11", "memory"
                    //);
                }
            }
        } else {
#endif
            std::array<value_type, 2> n{};
            value_type carry{};

            for (; b_first != b_last; ++b_first, ++out_first) {
                const auto digit = *b_first;
                if (digit == 0)
                    continue;
                auto in_first{a_first};
                auto ret_first{out_first};
                carry = 0;

                while (in_first != a_last) {
                    n = base_mul(*(in_first++), digit);

                    *ret_first += n[0];
                    carry += carry ? *(ret_first++) <= n[0] : *(ret_first++) < n[0];

                    *ret_first += n[1] + carry;
                    carry = carry ? *ret_first <= n[1] : *ret_first < n[1];
                }
            }
#if defined(XENONIS_INLINE_ASM_AMD64)
        }
#endif
    }

    // simple but inefficient implementation of the naive multiplication in AMD64 assembly
    // template <class OutIter, class InIter>
    // void simple_asm_naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last, OutIter out_first)
    //{
    // const auto a_size{std::distance(a_first, a_last)};

    // for (; b_first != b_last; ++b_first, ++out_first) {
    // const auto digit{*b_first};
    // if (digit == 0)
    // continue;

    // asm(R"(
    // xor %%rax, %%rax
    // xor %%rsi, %%rsi
    // mov %[digit], %%rdx
    //%=1:
    // mulx (%[in],%%rsi,8), %%r8, %%r9
    // mov (%[out],%%rsi,8), %%r10
    // mov 8(%[out],%%rsi,8), %%r11
    // add %%r8, %%r10
    // mov %%r10, (%[out],%%rsi,8)
    // adcx %%rax, %%r11
    // adcx %%r9, %%r11
    // mov %%r11, 8(%[out],%%rsi,8)

    // setc %%al
    // movzx %%al, %%rax
    // inc %%rsi
    // cmp %[size], %%rsi
    // jl %=1b
    //)"
    //: [ out ] "+r"(out_first)
    //: [ in ] "r"(a_first), [ digit ] "rm"(digit), [ size ] "r"(a_size)
    //: "rax", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory");
    //}
    //}

    template <class OutContainer, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        OutContainer
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        const auto a_size{std::distance(a_first, a_last)};
        const auto b_size{std::distance(b_first, b_last)};

        OutContainer ret(a_size + b_size, 0);

        naive_mul(a_first, a_last, b_first, b_last, ret.begin());

        remove_zeros(ret);
        return ret;
    }

    template <class OutContainer, class InIter>
#if !defined(XENONIS_INLINE_ASM_AMD64)
    constexpr
#endif
        OutContainer
        karatsuba_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        const auto a_size{std::distance(a_first, a_last)};
        const auto b_size{std::distance(b_first, b_last)};

        // TODO: test different thresholds for when to use the naive multiplication
        if (a_size <= 1024 || b_size <= 1024) {
            if (a_size < b_size) {
                return naive_mul<OutContainer>(b_first, b_last, a_first, a_last);
            } else {
                return naive_mul<OutContainer>(a_first, a_last, b_first, b_last);
            }
        }

        auto max_size{std::max(a_size, b_size)};
        if (max_size % 2 == 1)
            ++max_size;
        const auto limb_size{max_size / 2};

        InIter a_l_first{a_first};
        InIter a_l_last;
        InIter b_l_first{b_first};
        InIter b_l_last;
        InIter a_h_first;
        InIter a_h_last;
        InIter b_h_first;
        InIter b_h_last;

        bool a_h_zero{false};
        bool b_h_zero{false};

        // std::size_t a_h_size;
        // std::size_t b_h_size;

        if (limb_size > a_size) {
            a_l_last = a_last;
            a_h_zero = true;
            // a_h_size = 0;
            a_h_first = nullptr;
            a_h_last = nullptr;
        } else {
            a_l_last = a_first + limb_size; // maybe - 1
            a_h_first = a_first + limb_size;
            a_h_last = a_last;
            a_h_zero = is_zero(a_h_first, a_h_last);
            // a_h_size = std::distance(a_h_first, a_h_last);
        }

        if (limb_size > b_size) {
            b_l_last = b_last;
            b_h_zero = true;
            // b_h_size = 0;
            b_h_first = nullptr;
            b_h_last = nullptr;
        } else {
            b_l_last = b_first + limb_size; // maybe - 1
            b_h_first = b_first + limb_size;
            b_h_last = b_last;
            b_h_zero = is_zero(b_h_first, b_h_last);
            // b_h_size = std::distance(b_h_first, b_h_last);
        }

        bool a_l_zero{is_zero(a_first, a_last)};
        bool b_l_zero{is_zero(b_l_first, b_l_last)};

        if (a_h_zero && a_l_zero)
            return OutContainer(1, 0);

        if (b_h_zero && b_l_zero)
            return OutContainer(1, 0);

        if (a_h_zero) { // a_l_zero is false
            if (b_h_zero)
                return karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last);

            if (b_l_zero) // b_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_h_first, b_h_last), limb_size);

            auto x{karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last)};
            auto y{karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last)};

            OutContainer ret(a_size + b_size, 0);

            std::copy(x.begin(), x.end(), ret.begin() + limb_size);

            if (algorithms::add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (b_h_zero) { // b_l_zero is false
            if (a_h_zero)
                return karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_l_first, a_l_last);

            if (a_l_zero) // a_h_zero is false
                return lshift<OutContainer, OutContainer>(
                    karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_h_first, a_h_last), limb_size);

            auto x{karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last)};
            auto y{karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_l_first, a_l_last)};

            OutContainer ret(a_size + b_size, 0);

            std::copy(x.begin(), x.end(), ret.begin() + limb_size);

            if (algorithms::add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (a_l_zero) { // a_h_zero is false
            if (b_l_zero)
                return lshift<OutContainer, OutContainer>(
                    karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last), max_size);

            auto x{karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last)};
            auto y{lshift<OutContainer, OutContainer>(
                karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last), limb_size)};

            OutContainer ret(a_size + b_size, 0);
            std::copy(x.begin(), x.end(), ret.begin() + max_size);

            if (algorithms::add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (b_l_zero) { // b_h_zero is false
            auto x{karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_h_first, a_h_last)};
            auto y{lshift<OutContainer, OutContainer>(
                karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last), limb_size)};

            OutContainer ret(a_size + b_size, 0);
            std::copy(x.begin(), x.end(), ret.begin() + max_size);

            if (algorithms::add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        OutContainer p1;
        OutContainer p2;
        OutContainer p3;

        // calculate p1 and p2
        // tbb::task_group tg; // simple parallelization which may be used in the future
        /*tg.run([&]() {*/ p1 = karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last); //});
        /*tg.run([&]() {*/ p2 = karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last); //});

        constexpr auto cp_add = [](auto a_first, auto a_last, auto b_first, auto b_last, auto a_size, auto b_size) {
            constexpr auto add = [](auto a_first, auto a_last, auto b_first, auto b_last, auto a_size, auto b_size) {
                OutContainer tmp(a_size + 1);
                tmp.back() = 0;
                if (xenonis::algorithms::add(a_first, b_first, b_last, tmp.begin())) {
                    std::copy(a_first + b_size, a_last, tmp.begin() + b_size);
                    xenonis::algorithms::increment(tmp.begin() + b_size, tmp.end());
                } else {
                    std::copy(a_first + b_size, a_last, tmp.begin() + b_size);
                    tmp.pop_back();
                }
                return /*std::move(*/ tmp /*)*/;
            };

            if (a_size >= b_size)
                return add(a_first, a_last, b_first, b_last, a_size, b_size);
            else
                return add(b_first, b_last, a_first, a_last, b_size, a_size);
        };

        // calculate p3
        auto p3_1{cp_add(a_l_first, a_l_last, a_h_first, a_h_last, limb_size, std::distance(a_h_first, a_h_last))};
        auto p3_2{cp_add(b_l_first, b_l_last, b_h_first, b_h_last, limb_size, std::distance(b_h_first, b_h_last))};

        /*tg.run([&]() {*/ p3 = karatsuba_mul<OutContainer>(p3_1.begin(), p3_1.end(), p3_2.begin(), p3_2.end()); //});
        // tg.wait();

        // TODO: do not decrement twice, decrement by 2 ones
        if (sub_from(p3.begin(), p1.begin(), p1.end()))
            decrement(p3.begin() + p1.size(), p3.end());

        if (sub_from(p3.begin(), p2.begin(), p2.end()))
            decrement(p3.begin() + p2.size(), p3.end());

        remove_zeros(p3);

        // calculate result
        OutContainer ret(a_size + b_size, 0);
        std::copy(p1.begin(), p1.end(), ret.begin() + max_size);

        // TODO: do not decrement twice, decrement by 2 ones
        if (add(ret.cbegin(), p2.cbegin(), p2.cend(), ret.begin()))
            increment(ret.begin() + p2.size(), ret.end());

        if (add(ret.cbegin() + limb_size, p3.cbegin(), p3.cend(), ret.begin() + limb_size))
            increment(ret.begin() + p3.size() + limb_size, ret.end());

        remove_zeros(ret);
        return ret;
    }
} // namespace xenonis::algorithms
