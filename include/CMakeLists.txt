# ---------- (C) 2018, 2019 fahaas ----------

set (BIGINT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/bigint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/compare.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/conversion.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/util.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/integer_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/container/bigint_data.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/container/data_view.hpp
    )

if (XENONIS_USE_OPENMP)
    set (EXTRA_LIBS gomp)
endif ()

configure_file (
    "${PROJECT_SOURCE_DIR}/include/bigint_config.hpp.in"
    "${PROJECT_BINARY_DIR}/bigint_config.hpp"
)

add_library (
    bigint
    INTERFACE
)

target_sources(
    bigint
    INTERFACE
        ${BIGINT_HEADERS}
    )

target_include_directories(bigint
    INTERFACE
    .
)

install (
    TARGETS bigint
    DESTINATION lib64
)
install (
    FILES ${BIGINT_HEADERS}
    DESTINATION include
)
