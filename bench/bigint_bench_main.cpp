// ---------- (C) 2018, 2019 fahaas ----------
#include <algorithms/arithmetic.hpp>
#include <benchmark/benchmark.h>
#include <bigint.hpp>
#include <gmpxx.h>
#include <iostream>
#include <random>
#include <string>

std::string gen_ran_num(std::size_t size)
{
    std::string ret(size, '\0');

    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<char> ran_dist(0, 15);

    auto conv = [](char c) -> char {
        if (c < 10)
            return c + 48;
        else
            return c + 87;
    };

    for (auto& c : ret)
        c = conv(ran_dist(ran_engine));

    return ret;
}

static void BM_add(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::hex_bigint64 b_a(gen_ran_num(static_cast<std::size_t>(state.range(0))));
    xenonis::hex_bigint64 b_b(gen_ran_num(static_cast<std::size_t>(state.range(0))));

    for (auto _ : state)
        benchmark::DoNotOptimize(b_a + b_b);
}
BENCHMARK(BM_add)->RangeMultiplier(2)->Range(1, 8 << 16)->Complexity(benchmark::oN);

static void BM_mul(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::hex_bigint64 b_a(gen_ran_num(static_cast<std::size_t>(state.range(0))));
    xenonis::hex_bigint64 b_b(gen_ran_num(static_cast<std::size_t>(state.range(0))));

    for (auto _ : state)
        benchmark::DoNotOptimize(b_a * b_b);
}
BENCHMARK(BM_mul)->RangeMultiplier(2)->Range(1, 8 << 16)->Complexity(benchmark::oNSquared);

static void BM_karatsuba_mul(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::hex_bigint64 b_a(gen_ran_num(static_cast<std::size_t>(state.range(0))));
    xenonis::hex_bigint64 b_b(gen_ran_num(static_cast<std::size_t>(state.range(0))));

    auto a = b_a.data();
    auto b = b_b.data();
    decltype(a) c;

    for (auto _ : state) {
        c = xenonis::algorithms::hex_karatsuba_mul<std::uint64_t>(a.begin(), a.end(), b.begin(), b.end());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in_bytes"] =
        benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
    state.counters["res_bytes"] = benchmark::Counter(c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults,
                                                     benchmark::Counter::kIs1024);
}
BENCHMARK(BM_karatsuba_mul)->RangeMultiplier(2)->Range(1, 8 << 16)->UseRealTime()->Complexity();

static void BM_gmp_mul(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));
    mpz_class mp_a(gen_ran_num(static_cast<std::size_t>(state.range(0))), 16);
    mpz_class mp_b(gen_ran_num(static_cast<std::size_t>(state.range(0))), 16);

    mpz_t c;
    mpz_init(c);

    for (auto _ : state) {
        mpz_mul(c, mp_a.get_mpz_t(), mp_b.get_mpz_t());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in_bytes"] = benchmark::Counter(mpz_size(mp_a.get_mpz_t()) * sizeof(mp_limb_t),
                                                    benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
    state.counters["res_bytes"] =
        benchmark::Counter(mpz_size(c) * sizeof(mp_limb_t), benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);

    mpz_clear(c);
}
BENCHMARK(BM_gmp_mul)->RangeMultiplier(2)->Range(1, 8 << 16)->Complexity();

static void BM_naive_mul(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));
    xenonis::hex_bigint64 b_a(gen_ran_num(static_cast<std::size_t>(state.range(0))));
    xenonis::hex_bigint64 b_b(gen_ran_num(static_cast<std::size_t>(state.range(0))));

    auto a = b_a.data();
    auto b = b_b.data();
    decltype(a) c;

    for (auto _ : state) {
        c = xenonis::algorithms::hex_naive_mul<decltype(a)>(a.begin(), a.end(), b.begin(), b.end());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in_bytes"] =
        benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
    state.counters["res_bytes"] = benchmark::Counter(c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults,
                                                     benchmark::Counter::kIs1024);
}
BENCHMARK(BM_naive_mul)->RangeMultiplier(8)->Range(1, 8 << 16)->Complexity();

BENCHMARK_MAIN();
