// ---------- (C) 2018, 2019 fahaas ----------
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
#include <cassert>
#include <cstddef>
#include <cstdint>
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
    /*constexpr*/ inline bool add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

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
    constexpr inline bool sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first);

    /*!
     *  Subtracts b from a and writes the result to a. Requires a.size() >= b.size().
     *  \param a_first iterator poiting to first element of a.
     *  \param b_first iterator pointing to first element of b. Could be const iterator.
     *  \param b_last iterator pointing to last element of b. Could be const iterator.
     *  \returns carry
     */
    template <class InIter, class InOutIter>
    constexpr inline bool sub_from(InOutIter a_first, InIter b_first, InIter b_last);

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
     *  \details Uses the naive method to multiply. Complexity: O(n^2)
     *  \param a_first iterator pointing to the first element of a.
     *  \param a_last iterator pointing to the last element of a.
     *  \param b_first iterator pointing to the first element of b.
     *  \param b_last iterator pointing to the last element of b.
     *  \returns the result
     */
    template <class OutContainer, class InIter>
    constexpr OutContainer naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last);

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
    template <class OutIter, class InIter, typename size_type = std::size_t>
#if !(defined(__GNUC__) && defined(__amd64__))
    constexpr
#endif
        void
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last, OutIter out_first, OutIter out_last,
                  size_type a_size);

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
    constexpr OutContainer karatsuba_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last);

    template <typename Value, class InContainer, class OutContainer = InContainer>
    OutContainer div(const InContainer& a, const InContainer& b);

    /*bool hex_add(const std::uint64_t* a, const std::uint64_t* b, std::uint64_t* c, std::size_t size)
    {
        std::uint64_t carry_in = 0, carry_out = 0;
        for (std::size_t i = 0; i < size; ++i) {
        // TODO: benchmark if rm or r is faster
        asm volatile("cmp %[carry_in], 0;\n\t"
                     "je 1f;\n\t"
                     "stc;\n\t"
                     "1:\n\t"
                     "mov rax, QWORD PTR [%[a]+%[i]*8];\n\t"
                     "adc rax, QWORD PTR [%[b]+%[i]*8];\n\t"
                     "mov QWORD PTR [%[c]+%[i]*8], rax;"
                     : [ c ] "+rm"(c), "=@ccc"(carry) // DO NOT USE = constraint when using pointer
                     : [ a ] "rm"(a), [ b ] "rm"(b), [ carry_in ] "rm"(carry_in), [ i ] "rm"(i)
                     : "rax", "rsi", "memory");
            carry_in = carry_out;
        }
        return carry_in;
    }

    bool hex_add(const std::uint64_t* a, const std::uint64_t* b, std::uint64_t* c, std::size_t size)
    {
        std::uint64_t carry;
        asm volatile(                                        // IN AT&T SYNTAX:
                        "xor rsi, rsi;\n"                    // "xorq %%rsi, %%rsi;\n
                        "xor rbx, rbx;\n"                    // "xorq %%rbx, %%rbx;\n"
                        "not rbx;\n"                         // "notq %%rbx;\n"
                        "sub rbx, %[size];\n"                // "subq %%rbx, %[size];\n"
                        "add rbx, 1;\n"                      // "addq %%rbx, %1;\n"
                        "lea r8, [%[a]];\n"
                        "lea r9, [%[b]];\n"
                        "lea r10, [%[c]];\n"
                        "1:\n"                               // "loop%=:\n"
                        "mov rax, QWORD PTR [r8+rsi*8];\n"   // "movq %%rax, (%[a], %%rsi, 8);\n"
                        "adc rax, QWORD PTR [r9+rsi*8];\n"   // "adcq %%rax, (%[b], %%rsi, 8);\n"
                        "mov QWORD PTR [r10+rsi*8], rax;\n"  // "movq (%[c], %%rsi, 8), %%rax;\n"
                        "inc rsi;\n"                         // "incq %%rsi;\n"
                        "mov rcx, rbx;\n"                    // "movq %%rcx, %%rbx;\n"
                        "adox rcx, rsi;\n"                   // "adoxq %%rcx, %%rsi;\n"
                        "jno 1b;\n"                          // "jno loop%=;\n"
                        //"adc rax, 0;\n"                    // "adcq %%rax, %0;\n\t"
                        //"mov %[carry], rax;\n"             // "movq %[carry], %%rax;\n"
                        : [ c ] "+r"(c), "=@ccc"(carry)[carry] "=r" (carry) // DO NOT USE = constraint when using
    pointer : [ a ] "r"(a), [ b ] "r"(b), [ size ] "r"(size) : "rax", "rbx", "rcx", "rsi", "r8", "r9", "r10", "memory");
        //TODO: unify loops
        if (size % 4 != 0) {
             asm volatile(
                        "xor rsi, rsi;\n"
                        "xor rax, rax;\n"
                        "mov rbx, %[size];\n"
                        "lea r8, [%[a]];\n"
                        "lea r9, [%[b]];\n"
                        "lea r10, [%[c]];\n"
                        "1:\n"
                        "add rax, QWORD PTR [r8+rsi*8];\n"
                        "add rax, QWORD PTR [r9+rsi*8];\n"
                        "mov QWORD PTR [r10+rsi*8], rax;\n"
                        "mov rax, 0;\n"
                        "adc rax, 0;\n"
                        "add rsi, 1;\n"
                        "cmp rsi, rbx;\n"
                        "jl 1b;\n"
                        "mov %[carry], rax;\n"
                        : [ c ] "+r"(c), [carry] "=r" (carry) // DO NOT USE = constraint when using pointer
                        : [ a ] "r"(a), [ b ] "r"(b), [ size ] "r"(size)
                        : "rax", "rbx", "rcx", "rsi", "r8", "r9", "r10", "r11", "r12", "memory");
            return carry;
        }
        asm volatile(
                        "xor rsi, rsi;\n"
                        "xor rax, rax;\n"
                        "mov rbx, %[size];\n"
                        "lea r8, [%[a]];\n"
                        "lea r9, [%[b]];\n"
                        "lea r10, [%[c]];\n"
                        "1:\n"
                        "add rax, QWORD PTR [r8+rsi*8];\n"
                        "add rax, QWORD PTR [r9+rsi*8];\n"
                        "mov QWORD PTR [r10+rsi*8], rax;\n"
                        "mov rax, QWORD PTR [r8+rsi*8+8];\n"
                        "adc rax, QWORD PTR [r9+rsi*8+8];\n"
                        "mov QWORD PTR [r10+rsi*8+8], rax;\n"
                        "mov rax, QWORD PTR [r8+rsi*8+16];\n"
                        "adc rax, QWORD PTR [r9+rsi*8+16];\n"
                        "mov QWORD PTR [r10+rsi*8+16], rax;\n"
                        "mov rax, QWORD PTR [r8+rsi*8+24];\n"
                        "adc rax, QWORD PTR [r9+rsi*8+24];\n"
                        "mov QWORD PTR [r10+rsi*8+24], rax;\n"
                        "mov rax, 0;\n"
                        "adc rax, 0;\n"
                        "add rsi, 4;\n"
                        "cmp rsi, rbx;\n"
                        "jl 1b;\n"
                        "mov %[carry], rax;\n"
                        : [ c ] "+r"(c), [carry] "=r" (carry) // DO NOT USE = constraint when using pointer
                        : [ a ] "r"(a), [ b ] "r"(b), [ size ] "r"(size)
                        : "rax", "rbx", "rcx", "rsi", "r8", "r9", "r10", "r11", "r12", "memory");
        return carry;
    }

    template <class InIter, class OutIter>
    bool r_add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        const auto size = std::distance(b_first, b_last);
        std::uint64_t carry;
        asm volatile(
                    "mov rsi, %[size];\n"
                    "dec rsi;\n"
                    "1:\n"
                    "mov rax, QWORD PTR [[%[a]]+rsi*8];\n"
                    "adc rax, QWORD PTR [[%[b]]+rsi*8];\n"
                    "mov QWORD PTR [[%[c]]+rsi*8], rax;\n"
                    "dec rsi;\n"
                    "jnz 1b;\n"
                    "mov rax, QWORD PTR [[%[a]]+rsi*8];\n"
                    "adc rax, QWORD PTR [[%[b]]+rsi*8];\n"
                    "mov QWORD PTR [[%[c]]+rsi*8], rax;\n"
                    : [ c ] "+r"(c_first), "=@ccc"(carry)
                    : [ a ] "r" (a_first), [ b ] "r" (b_first), [ size ] "r" (size)
                    : "rax", "rbx", "rcx", "rsi", "memory");
        return carry;
    }

    template <class InIter, class OutIter>
    bool inc_add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        std::uint64_t carry, size = std::distance(b_first, b_last);
        asm volatile(
                    "xor rsi, rsi;\n"
                    "mov rbx, %[max];\n"
                    "sub rbx, %[size];\n"
                    "add rbx, 1;\n"
                    "1:\n"
                    "mov rax, QWORD PTR [%[a]+rsi*8];\n"
                    "adc rax, QWORD PTR [%[b]+rsi*8];\n"
                    "mov QWORD PTR [%[c]+rsi*8], rax;\n"
                    "inc rsi;\n"
                    "inc rbx;\n"
                    "jno 1b;\n"
                    : [ c ] "+r"(c_first), "=@ccc"(carry) // DO NOT USE = constraint when using pointer
                    : [ a ] "r" (a_first), [ b ] "r" (b_first), [ size ] "r" (size),  [ max ] "r" (INT64_MAX)
                    : "rax", "rbx", "rcx", "rsi", "memory");
        return carry;
    }*/

    /*!
     *  Adds b to a and writes the result to c. Requires a.size() >= b.size() and c.size() >= a.size(). It is possible
     *  that c is a or b. IMPORTANT: size must not be < 4
     *  \param a pointer to the first element of a. Could be equal c.
     *  \param b pointer to the first element of b. Could be equal c.
     *  \param c pointer to the first element of c. Could be equal a or b.
     *  \param size the size of the arrays
     *  \returns carry
     */
    extern "C" std::uint64_t XENONIS_add(const std::uint64_t* a, const std::uint64_t* b, std::uint64_t* c,
                                         std::uint64_t size);

    template <class InIter, class OutIter>
#if !(defined(__GNUC__) && defined(__amd64__))
    constexpr
#endif
        bool inline add(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
#if defined(__GNUC__) && defined(__amd64__)
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;

        if constexpr (std::is_same_v<value_type, std::uint64_t>) {
            std::uint64_t carry = 0, size = std::distance(b_first, b_last);
            if (size < 4) {
                asm(R"(
                xor %%rsi, %%rsi
                movq %[max], %%rbx
                subq %[size], %%rbx
                addq $1, %%rbx
                %=1:
                movq (%[a], %%rsi, 8), %%rax
                adcq (%[b], %%rsi, 8), %%rax
                movq %%rax, (%[c], %%rsi, 8)
                inc %%rsi
                inc %%rbx
                jno %=1b
                adcq $0, %[carry]
                )"
                    : [ c ] "+r"(c_first), [ carry ] "+r"(carry) // DO NOT USE = constraint when using pointer
                    : [ a ] "r"(a_first), [ b ] "r"(b_first), [ size ] "r"(size),
                      [ max ] "r"(std::numeric_limits<std::int64_t>::max())
                    : "rax", "rbx", "rcx", "rsi", "memory");

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
                return carry;
            } else {
                // TODO: implement using inline assembly, instead of calling assembly, improves portability, because
                // there is no need to take care of the calling conventions of Unix and Windows
                carry = XENONIS_add(a_first, b_first, c_first, size);
            }
            return carry;
        }
#elif defined(__clang__) // for different architectures than x86
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
#if __has_builtin(__builtin_addcll)
        if constexpr (std::is_same_v<value_type, unsigned long long>) {
            unsigned long long carry_in = 0, carry_out = 0;
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addcll(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#if __has_builtin(__builtin_addcl)
        if constexpr (std::is_same_v<value_type, unsigned long>) {
            unsigned long carry_in = 0, carry_out = 0;
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addcl(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#if __has_builtin(__builtin_addc)
        if constexpr (std::is_same_v<value_type, unsigned int>) {
            unsigned int carry_in = 0, carry_out = 0;
            for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
                *c_first = __builtin_addc(*a_first, *b_first, carry_in, &carry_out);
                carry_in = carry_out;
            }
            return carry_in;
        }
#endif
#endif
        // portable
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first + *b_first + carry;
            carry = carry ? *c_first <= *b_first : *c_first < *b_first;
        }
        return carry;
    }

    template <class InIter, class OutIter>
    constexpr inline bool sub(InIter a_first, InIter b_first, InIter b_last, OutIter c_first)
    {
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first, ++c_first) {
            *c_first = *a_first - *b_first - carry;
            carry = carry ? *c_first >= *a_first : *c_first > *a_first;
        }
        return carry;
    }

    template <class InIter, class InOutIter>
    constexpr inline bool sub_from(InOutIter a_first, InIter b_first, InIter b_last)
    {
        bool carry = false;
        for (; b_first != b_last; ++a_first, ++b_first) {
            auto tmp = *a_first;
            *a_first -= *b_first + carry;
            carry = carry ? *a_first >= tmp : *a_first > tmp;
        }
        return carry;
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

    template <class OutIter, class InIter, typename size_type = std::size_t>
#if !(defined(__GNUC__) && defined(__amd64__))
    constexpr
#endif
        void
        naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last, OutIter out_first, OutIter out_last)
    {
#if defined(__GNUC__) && defined(__amd64__)
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;

        const auto a_size = std::distance(a_first, a_last);
        value_type carry{0}, i_out{0};

        for (; b_first != b_last; ++b_first, ++out_first) {
            const auto digit = *b_first;
            if (digit == 0)
                continue;

            if (a_size % 2 == 0) {
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

                mov $0, %[carry]
                adcx %%rax, %[carry]
                adox %%rax, %[carry]

                add $1, %%rsi
                mov %%rsi, %[i_out]
                )"
                    : [ out ] "+r"(out_first), [ carry ] "+r"(carry), [ i_out ] "=rm"(i_out)
                    : [ in ] "r"(a_first), [ digit ] "rm"(digit),
                      [ size ] "r"(a_size - static_cast<size_type>(a_size % 2 == 0))
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
                //
                // mov %[carry], 0
                // adcx %[carry], rax
                // adox %[carry], rax
                //
                // add rsi, 1
                // mov %[i_out], rsi
                //)"
                //: [out] "+r" (out_first), [carry] "+r" (carry), [i_out] "=rm" (i_out)
                //: [in] "r" (a_first), [digit] "rm" (digit), [size] "r" (a_size - static_cast<size_type>(a_size % 2 ==
                // 0)) : "rax", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "memory"
                //);
            } else {
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

                mov $0, %%rax
                mov $0, %[carry]
                adcx %%rax, %[carry]
                adox %%rax, %[carry]

                mov %%rsi, %[i_out]
                )"
                    : [ out ] "+r"(out_first), [ carry ] "+r"(carry), [ i_out ] "=rm"(i_out)
                    : [ in ] "r"(a_first), [ digit ] "rm"(digit), [ size ] "r"(a_size)
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
                //
                // mov rax, 0
                // mov %[carry], 0
                // adcx %[carry], rax
                // adox %[carry], rax
                //
                // mov %[i_out], rsi
                //)"
                //: [out] "+r" (out_first), [carry] "+r" (carry), [i_out] "=rm" (i_out)
                //: [in] "r" (a_first), [digit] "rm" (digit), [size] "r" (a_size)
                //: "rax", "rcx", "rdx", "rbx", "rsi", "r8", "r9", "r10", "r11", "memory"
                //);
            }

            if (carry)
                increment(out_first + i_out, out_last);
        }
#else
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
        using doubled = typename traits::uint<value_type>::doubled;

        doubled n0 = 0;
        doubled n1 = 0;
        auto* n0_ptr = reinterpret_cast<value_type*>(&n0);
        auto* n1_ptr = reinterpret_cast<value_type*>(&n1);

        value_type carry{0};

        for (; b_first != b_last; ++b_first, ++out_first) {
            const auto digit = *b_first;
            if (digit == 0)
                continue;
            auto in_first = a_first;
            auto ret_first = out_first;

            n0 = static_cast<doubled>(*(in_first++)) * digit;
            *ret_first += n0_ptr[0];
            carry = *(ret_first++) < n0_ptr[0];

            const auto in_last = a_last - (a_size % 2 == 0);
            while (in_first != in_last) {
                n1 = static_cast<doubled>(*(in_first++)) * digit;
                *ret_first += n0_ptr[1] + carry;
                carry = carry ? *ret_first <= n0_ptr[1] : *ret_first < n0_ptr[1];
                *ret_first += n1_ptr[0];
                carry += carry ? *(ret_first++) <= n1_ptr[0] : *(ret_first++) < n1_ptr[0];

                n0 = static_cast<doubled>(*(in_first++)) * digit;
                *ret_first += n1_ptr[1] + carry;
                carry = carry ? *ret_first <= n1_ptr[1] : *ret_first < n1_ptr[1];
                *ret_first += n0_ptr[0];
                carry += carry ? *(ret_first++) <= n0_ptr[0] : *(ret_first++) < n0_ptr[0];
            }

            if (a_size % 2 == 0) {
                n1 = static_cast<doubled>(*(in_first++)) * digit;
                *ret_first += n0_ptr[1] + carry;
                carry = carry ? *ret_first <= n0_ptr[1] : *ret_first < n0_ptr[1];
                *ret_first += n1_ptr[0];
                carry += carry ? *(ret_first++) <= n1_ptr[0] : *(ret_first++) < n1_ptr[0];

                *ret_first += n1_ptr[1] + carry;

                if (carry) {
                    if (*(ret_first++) <= n1_ptr[1])
                        increment(ret_first, out_last);
                } else {
                    if (*(ret_first++) < n1_ptr[1])
                        increment(ret_first, out_last);
                }
            } else {
                *ret_first += n0_ptr[1] + carry;

                if (carry) {
                    if (*(ret_first++) <= n0_ptr[1])
                        increment(ret_first, out_last);
                } else {
                    if (*(ret_first++) < n0_ptr[1])
                        increment(ret_first, out_last);
                }
            }
        }
#endif
    }

    template <class OutContainer, class InIter>
    constexpr OutContainer naive_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        const auto a_size = std::distance(a_first, a_last);
        const auto b_size = std::distance(b_first, b_last);

        OutContainer ret(a_size + b_size, 0);

        naive_mul(a_first, a_last, b_first, b_last, ret.begin(), ret.end());

        remove_zeros(ret);
        return ret;
    }

    template <class OutContainer, class InIter>
    constexpr OutContainer karatsuba_mul(InIter a_first, InIter a_last, InIter b_first, InIter b_last)
    {
        using value_type = std::remove_const_t<std::remove_reference_t<decltype(*a_first)>>;
        using doubled = typename traits::uint<value_type>::doubled;

        const auto a_size = std::distance(a_first, a_last);
        const auto b_size = std::distance(b_first, b_last);

        // TODO: test different thresholds for when to use the naive multiplication
        if (a_size <= 1024 || b_size <= 1024) {
            if (a_size < b_size) {
                return naive_mul<OutContainer>(b_first, b_last, a_first, a_last);
            } else {
                return naive_mul<OutContainer>(a_first, a_last, b_first, b_last);
            }
        }
        /*if (a_size == 1 || b_size == 1) {
            if (a_size == b_size) {
                OutContainer tmp(2);
                doubled ret = static_cast<doubled>(*a_first) * (*b_first);
                auto* ret_ptr = reinterpret_cast<value_type*>(&ret);
                tmp[0] = ret_ptr[0];
                tmp[1] = ret_ptr[1];
                return tmp;
            } else if (a_size < b_size) {
                return naive_mul<OutContainer>(b_first, b_last, a_first, a_last);
            } else {
                return naive_mul<OutContainer>(a_first, a_last, b_first, b_last);
            }
        }*/

        auto max_size = std::max(a_size, b_size);
        if (max_size % 2 == 1)
            ++max_size;
        const auto limb_size = max_size / 2;

        InIter a_l_first = a_first;
        InIter a_l_last;
        InIter b_l_first = b_first;
        InIter b_l_last;
        InIter a_h_first;
        InIter a_h_last;
        InIter b_h_first;
        InIter b_h_last;

        bool a_h_zero = false;
        bool b_h_zero = false;

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

        bool a_l_zero = is_zero(a_first, a_last);
        bool b_l_zero = is_zero(b_l_first, b_l_last);

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

            auto x = karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last);
            auto y = karatsuba_mul<OutContainer>(a_l_first, a_l_last, b_l_first, b_l_last);

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

            auto x = karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last);
            auto y = karatsuba_mul<OutContainer>(b_l_first, b_l_last, a_l_first, a_l_last);

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

            auto x = karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_h_first, b_h_last);
            auto y = lshift<OutContainer, OutContainer>(
                karatsuba_mul<OutContainer>(a_h_first, a_h_last, b_l_first, b_l_last), limb_size);

            OutContainer ret(a_size + b_size, 0);
            std::copy(x.begin(), x.end(), ret.begin() + max_size);

            if (algorithms::add(ret.cbegin(), y.cbegin(), y.cend(), ret.begin()))
                algorithms::increment(ret.begin() + y.size(), ret.end());

            remove_zeros(ret);
            return ret;
        }

        if (b_l_zero) { // b_h_zero is false
            auto x = karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_h_first, a_h_last);
            auto y = lshift<OutContainer, OutContainer>(
                karatsuba_mul<OutContainer>(b_h_first, b_h_last, a_l_first, a_l_last), limb_size);

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
        auto p3_1 = cp_add(a_l_first, a_l_last, a_h_first, a_h_last, limb_size, std::distance(a_h_first, a_h_last));
        auto p3_2 = cp_add(b_l_first, b_l_last, b_h_first, b_h_last, limb_size, std::distance(b_h_first, b_h_last));

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
