#------------------------------------------------------------------------------- 
# Copyright 2018-2020 Fabian Haas
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#------------------------------------------------------------------------------- 

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
