#------------------------------------------------------------------------------- 
# Copyright 2018-2020 Fabian Haas
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#------------------------------------------------------------------------------- 

add_executable(bigint_bench bigint_bench_main.cpp)

target_link_libraries(bigint_bench bigint gmp gmpxx benchmark pthread)

if(MSVC)
  target_compile_options(bigint_bench PRIVATE $<$<CONFIG:Release>:/O2 /GR-
                                              /DNDEBUG>)
  target_compile_options(bigint_bench PRIVATE /W4)
else()
  target_compile_options(bigint_bench PRIVATE $<$<CONFIG:Release>:-O3 -fno-rtti
                                              -DNDEBUG>)
  target_compile_options(bigint_bench PRIVATE -Wall -Wextra)
endif()
