#------------------------------------------------------------------------------- 
# Copyright 2018-2020 Fabian Haas
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#------------------------------------------------------------------------------- 

configure_file("${PROJECT_SOURCE_DIR}/include/bigint_config.hpp.in"
               "${PROJECT_BINARY_DIR}/bigint_config.hpp")

set(BIGINT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/bigint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/compare.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/conversion.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/util.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/integer_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/container/bigint_data.hpp
    ${PROJECT_BINARY_DIR}/bigint_config.hpp)

add_library(bigint INTERFACE)

target_sources(bigint INTERFACE ${BIGINT_HEADERS})

target_include_directories(bigint INTERFACE .)

target_compile_features(bigint INTERFACE cxx_std_17)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/bigint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/integer_traits.hpp
        ${PROJECT_BINARY_DIR}/bigint_config.hpp 
        ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION include/bigint)
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/arithmetic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/compare.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/conversion.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/util.hpp
  DESTINATION include/bigint/algorithms)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/container/bigint_data.hpp
        DESTINATION include/bigint/container)
