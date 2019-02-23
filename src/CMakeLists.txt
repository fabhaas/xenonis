# ---------- (C) 2018 fahaas ----------

set (BIGINT_HEADERS bigint.hpp bigint_base.hpp algorithms.hpp integer_traits.hpp bigint_data.hpp)
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

target_include_directories(bigint
    INTERFACE
    ${BIGINT_HEADERS}
)

install (
    TARGETS bigint
    DESTINATION lib64
)
install (
    FILES ${BIGINT_EXPORT_HEADERS}
    DESTINATION include
)
