#include "../src/bigint.hpp"
#include <array>
#include <chrono>
#include <geiger/geiger.h>
#include <gmp.h>
#include <string>

constexpr std::array<std::uint64_t, 5> ran_numbers = {3, 7, 13, 17, 23};
constexpr std::array<std::uint64_t, 2> bases = {10, 16};
constexpr std::array<std::uint64_t, 5> exponents = {1, 10, 100, 1000, 10000};

#define BIGINT_BENCH(bigint_t, bigint_t_name, name, op)                                                                \
    auto name##_bench = [&s](auto ran, auto base, auto exp) {                                                          \
        mpz_t mp_a;                                                                                                    \
        mpz_t mp_b;                                                                                                    \
        mpz_init(mp_a);                                                                                                \
        mpz_init(mp_b);                                                                                                \
                                                                                                                       \
        mpz_ui_pow_ui(mp_a, base, exp);                                                                                \
        /*mpz_mul_si(mp_a, mp_a, sign);*/                                                                              \
        mpz_mul_ui(mp_a, mp_a, ran);                                                                                   \
                                                                                                                       \
        mpz_ui_pow_ui(mp_b, base, exp);                                                                                \
        mpz_mul_ui(mp_b, mp_b, ran);                                                                                   \
                                                                                                                       \
        bigint_t a(mpz_get_str(NULL, 16, mp_a));                                                                       \
        bigint_t b(mpz_get_str(NULL, 16, mp_b));                                                                       \
                                                                                                                       \
        mpz_clear(mp_a);                                                                                               \
        mpz_clear(mp_b);                                                                                               \
                                                                                                                       \
        s.add(std::string(#bigint_t_name) + '_' + #name + '_' + std::to_string(base) + '^' + std::to_string(exp) +     \
                  '*' + std::to_string(ran),                                                                           \
              [a = std::move(a), b = std::move(b)]() { a op b; });                                                     \
    };

int main(/*int argc, char** argv*/)
{
    geiger::init();
    geiger::suite<> s;

    BIGINT_BENCH(numeric::bigint<std::uint64_t>, u64, add, +);
    BIGINT_BENCH(numeric::bigint<std::uint64_t>, u64, mult, *);

    auto add_benches = [&](auto& bench) {
        for (auto base : bases)
            for (auto exp : exponents)
                for (auto ran : ran_numbers)
                    bench(ran, base, exp);
    };

    add_benches(add_bench);
    add_benches(mult_bench);

    s.set_printer<geiger::printer::console<>>();
    s.run();

    return 0;
}
