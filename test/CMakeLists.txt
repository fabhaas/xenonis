# ---------- (C) 2018, 2019 fahaas ----------
add_executable (
	_bigint_test
	bigint_test_main.cpp
)

target_link_libraries (
	_bigint_test
	bigint
        ${XENONIS_REQUIRED_LIBS}
	gmp
        gmpxx
	${CONAN_LIBS}
)

add_test (
	NAME bigint_test
	COMMAND _bigint_test
)
