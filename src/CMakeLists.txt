# ---------- (C) 2018 fahaas ----------

set (BIGINT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/bigint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithms.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/integer_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bigint_data.hpp)

set (BIGINT_EXPORT_HEADERS bigint.hpp)

if (USE_OPENMP)
    set (EXTRA_LIBS gomp)
endif ()

configure_file (
    "${PROJECT_SOURCE_DIR}/src/bigint_config.hpp.in"
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
