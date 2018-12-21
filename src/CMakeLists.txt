# ---------- (C) 2018 fahaas ----------

set (BIGINT_SOURCES bigint.cpp)
set (BIGINT_HEADERS bigint.hpp algorithms.hpp integer_traits.hpp)
set (BIGINT_EXPORT_HEADERS bigint.hpp)

configure_file (
    "${PROJECT_SOURCE_DIR}/src/bigint_config.hpp.in"
    "${PROJECT_BINARY_DIR}/bigint_config.hpp"
)

add_library (
	bigint
        ${BIGINT_SOURCES}
        ${BIGINT_HEADERS}
)

target_link_libraries (
	bigint
	${CONAN_LIBS}
)

install (
    TARGETS bigint
    DESTINATION lib64
)
install (
    FILES ${BIGINT_EXPORT_HEADERS}
    DESTINATION include
)
