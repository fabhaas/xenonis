#include "../src/bigint.hpp"
#include <array>
#include <geiger/geiger.h>
#include <gmp.h>
#include <gmpxx.h>
#include <string>

constexpr std::array<std::uint64_t, 5> ran_numbers = {3, 7, 13, 17, 23};
constexpr std::array<std::uint64_t, 2> bases = {10, 256};
constexpr std::array<std::uint64_t, 7> exponents = {1,       4,       128,
                                                    64 << 4, 64 << 8, 64 << 16 /*, 64l << 20*/};

#define BIGINT_OPERATOR_BENCH(bigint_t, bigint_t_name, name, op)                                   \
    auto name##_bench = [&s](auto ran, auto base, auto exp) {                                      \
        mpz_class a;                                                                               \
        mpz_class b;                                                                               \
                                                                                                   \
        mpz_ui_pow_ui(a.get_mpz_t(), base, exp);                                                   \
        a *= ran;                                                                                  \
                                                                                                   \
        mpz_ui_pow_ui(b.get_mpz_t(), base - 1, exp);                                               \
        b *= (ran + 1);                                                                            \
                                                                                                   \
        bigint_t b_a(a.get_str(16));                                                               \
        bigint_t b_b(b.get_str(16));                                                               \
                                                                                                   \
        s.add(std::string(#bigint_t_name) + '_' + #name + '_' + std::to_string(base) + '^' +       \
                  std::to_string(exp) + '*' + std::to_string(ran) + '_' +                          \
                  std::to_string(b_a.size()),                                                      \
              [a = std::move(b_a), b = std::move(b_b)]() { a op b; });                             \
    };

int main(/*int argc, char** argv*/)
{
    geiger::init();
    geiger::suite<> s;

    BIGINT_OPERATOR_BENCH(xenonis::hex_bigint, u64, add, +);
    BIGINT_OPERATOR_BENCH(xenonis::hex_bigint, u64, sub, -);
    BIGINT_OPERATOR_BENCH(xenonis::hex_bigint, u64, mul, *);

    auto add_benches = [&](auto& bench) {
        for (auto base : bases)
            for (auto exp : exponents)
                for (auto ran : ran_numbers)
                    bench(ran, base, exp);
    };

    add_benches(add_bench);
    add_benches(sub_bench);
    add_benches(mul_bench);

    s.set_printer<geiger::printer::console<>>();
    s.run();

    return 0;
}
