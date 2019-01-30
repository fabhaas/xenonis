add_executable(
    bigint_bench
    bigint_bench_main.cpp
)

target_link_libraries(
    bigint_bench
    bigint
    gmp
)
