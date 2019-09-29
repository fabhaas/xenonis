// ---------- (C) 2018, 2019 fahaas ----------
#include <algorithms/arithmetic.hpp>
#include <benchmark/benchmark.h>
#include <bigint.hpp>
#include <functional>
#include <gmpxx.h>
#include <iostream>
#include <random>
#include <string>

template <typename T> auto gen_ran_nums(std::size_t size)
{
    std::random_device ran_device;
    std::default_random_engine ran_engine(ran_device());
    std::uniform_int_distribution<T> ran_dist(0, std::numeric_limits<T>::max());
    std::vector<T> ret(size);

    for (auto& e : ret)
        e = ran_dist(ran_engine);

    return ret;
}

auto gen_ran_hex_str(std::size_t size)
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

static void fibonacci_gen(std::function<void(int, std::size_t)> f)
{
    std::set<int> values;
    std::size_t i = 0;
    int a = 2;
    int b = 1;
    int tmp;
    do {
        tmp = a;
        a += b;
        b = tmp;
        values.insert(a - a / 8);
        values.insert(a);
        values.insert(a + a / 8);
    } while (a < (8 << 16));

    for (auto e : values)
        f(e, i++);
}

static void fibonacci_args(benchmark::internal::Benchmark* bench)
{
    fibonacci_gen([&bench](int n, std::size_t i) { bench->Args({n, static_cast<long>(i)}); });
    return;
}

static void fibonacci_offset_gen(std::function<void(int, std::size_t)> f)
{
    std::size_t i = 0;
    int a = 8 << 16;
    int b = 1;
    int tmp;
    do {
        tmp = a;
        a += b;
        b = tmp;
        f(a, i);
        ++i;
    } while (a < (8 << 22));
}

static void fibonacci_offset_args(benchmark::internal::Benchmark* bench)
{
    fibonacci_offset_gen([&bench](int n, std::size_t i) { bench->Args({n, static_cast<long>(i)}); });
    return;
}

static void p2_gen(std::function<void(int, std::size_t)> f)
{
    std::set<int> values;
    int n = 16;
    while (n < (8 << 18)) {
        n *= 2;
        values.insert(n - n / 4);
        values.insert(n);
        values.insert(n + n / 4);
    }

    std::size_t i = 0;
    for (auto e : values)
        f(e, i++);
}

static void p2_args(benchmark::internal::Benchmark* bench)
{
    p2_gen([&bench](int n, std::size_t i) { bench->Args({n, static_cast<long>(i)}); });
    return;
}

std::vector<std::pair<std::string, std::string>> add_data;
std::vector<std::pair<std::string, std::string>> mul_data;

void init()
{
    fibonacci_offset_gen([](int n, std::size_t) {
        add_data.push_back(
            std::make_pair(gen_ran_hex_str(static_cast<std::size_t>(n)), gen_ran_hex_str(static_cast<std::size_t>(n))));
    });
    p2_gen([](int n, std::size_t) {
        mul_data.push_back(
            std::make_pair(gen_ran_hex_str(static_cast<std::size_t>(n)), gen_ran_hex_str(static_cast<std::size_t>(n))));
    });
}

static void BM_add(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::bigint64 b_a(add_data.operator[](static_cast<std::size_t>(state.range(1))).first);
    xenonis::bigint64 b_b(add_data.operator[](static_cast<std::size_t>(state.range(1))).second);

    auto a = b_a.data();
    auto b = b_b.data();

    decltype(a) c(a.size() + 1);

    for (auto _ : state) {
        if (xenonis::algorithms::add(a.begin(), b.begin(), b.end(), c.begin()))
            c.back() = 1;
        benchmark::DoNotOptimize(c);
        // b_c = b_a + b_b;
        // benchmark::DoNotOptimize(b_c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] = benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults);
    state.counters["res_bytes"] = benchmark::Counter(c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults);
}
BENCHMARK(BM_add)->Apply(fibonacci_offset_args)->Complexity(benchmark::oN);

static void BM_add_gmp(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    mpz_class mp_a(add_data.operator[](static_cast<std::size_t>(state.range(1))).first, 16);
    mpz_class mp_b(add_data.operator[](static_cast<std::size_t>(state.range(1))).second, 16);

    mpz_t c;
    mpz_init(c);
    mpz_realloc(c, mpz_size(mp_a.get_mpz_t()) + 1);

    for (auto _ : state) {
        mpz_add(c, mp_a.get_mpz_t(), mp_b.get_mpz_t());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] = benchmark::Counter(mpz_size(mp_a.get_mpz_t()) * sizeof(mp_limb_t),
                                                    benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);
    state.counters["res_bytes"] = benchmark::Counter(mpz_size(c) * sizeof(mp_limb_t),
                                                     benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);

    mpz_clear(c);
}
BENCHMARK(BM_add_gmp)->Apply(fibonacci_offset_args)->Complexity(benchmark::oN);

static void BM_mul(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::bigint64 b_a(mul_data.operator[](static_cast<std::size_t>(state.range(1))).first);
    xenonis::bigint64 b_b(mul_data.operator[](static_cast<std::size_t>(state.range(1))).second);

    decltype(b_a) b_c;

    for (auto _ : state) {
        b_c = b_a * b_b;
        benchmark::DoNotOptimize(b_c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] =
        benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);
    state.counters["res_bytes"] = benchmark::Counter(b_c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults/*,
                                                     benchmark::Counter::kIs1024*/);
}
BENCHMARK(BM_mul)->Apply(p2_args)->Complexity(benchmark::oNSquared);

static void BM_mul_karatsuba(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::bigint64 b_a(mul_data.operator[](static_cast<std::size_t>(state.range(1))).first);
    xenonis::bigint64 b_b(mul_data.operator[](static_cast<std::size_t>(state.range(1))).second);

    auto a = b_a.data();
    auto b = b_b.data();
    decltype(a) c;

    for (auto _ : state) {
        c = xenonis::algorithms::karatsuba_mul<decltype(a)>(a.begin(), a.end(), b.begin(), b.end());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] =
        benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);
    state.counters["res_bytes"] = benchmark::Counter(c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults/*,
                                                     benchmark::Counter::kIs1024*/);
}
BENCHMARK(BM_mul_karatsuba)->Apply(p2_args)->Complexity();

static void BM_mul_naive(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    xenonis::bigint64 b_a(mul_data.operator[](static_cast<std::size_t>(state.range(1))).first);
    xenonis::bigint64 b_b(mul_data.operator[](static_cast<std::size_t>(state.range(1))).second);

    auto a = b_a.data();
    auto b = b_b.data();
    decltype(a) c;

    for (auto _ : state) {
        c = xenonis::algorithms::naive_mul<decltype(a)>(a.begin(), a.end(), b.begin(), b.end());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] =
        benchmark::Counter(b_a.size(), benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);
    state.counters["res_bytes"] = benchmark::Counter(c.size() * sizeof(std::uint64_t), benchmark::Counter::kDefaults/*,
                                                     benchmark::Counter::kIs1024*/);
}
BENCHMARK(BM_mul_naive)->Apply(p2_args)->Complexity();

static void BM_mul_gmp(benchmark::State& state)
{
    state.SetComplexityN(state.range(0));

    mpz_class mp_a(mul_data.operator[](static_cast<std::size_t>(state.range(1))).first, 16);
    mpz_class mp_b(mul_data.operator[](static_cast<std::size_t>(state.range(1))).second, 16);

    mpz_t c;
    mpz_init(c);

    for (auto _ : state) {
        mpz_mul(c, mp_a.get_mpz_t(), mp_b.get_mpz_t());
        benchmark::DoNotOptimize(c);
    }

    state.counters["in"] = state.range(0);
    state.counters["in_bytes"] = benchmark::Counter(mpz_size(mp_a.get_mpz_t()) * sizeof(mp_limb_t),
                                                    benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);
    state.counters["res_bytes"] = benchmark::Counter(mpz_size(c) * sizeof(mp_limb_t),
                                                     benchmark::Counter::kDefaults /*, benchmark::Counter::kIs1024*/);

    mpz_clear(c);
}
BENCHMARK(BM_mul_gmp)->Apply(p2_args)->Complexity();

int main(int argc, char** argv)
{
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    init();
    ::benchmark::RunSpecifiedBenchmarks();
}
