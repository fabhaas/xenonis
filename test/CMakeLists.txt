# ---------- (C) 2018-2020 Fabian Haas ----------

add_executable(bigint_test bigint_test_main.cpp)

target_link_libraries(bigint_test bigint gmp gmpxx ${CONAN_LIBS})

if(MSVC)
  target_compile_options(bigint_test PRIVATE $<$<CONFIG:Release>:/O2>)
  target_compile_options(bigint_test PRIVATE /W4)
else()
  target_compile_options(bigint_test PRIVATE $<$<CONFIG:Release>:-O3>)
  target_compile_options(bigint_test PRIVATE -Wall -Wextra)
endif()

add_test(NAME bigint_test COMMAND bigint_test)
