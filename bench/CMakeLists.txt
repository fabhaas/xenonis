# ---------- (C) 2018, 2019 fahaas ----------
add_executable(
    bigint_bench
    bigint_bench_main.cpp
    )

target_link_libraries(
    bigint_bench
    bigint
    ${XENONIS_REQUIRED_LIBS}
    gmp
    gmpxx
    benchmark
    pthread
    )
