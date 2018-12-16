#(C) 2018 fahaas
add_executable(
	_bigint_test
	bigint_test_main.cpp
)

target_link_libraries(
	_bigint_test
	${CONAN_LIBS}
	#bigint
)

enable_testing()

add_test(
	bigint_test
	_bigint_test
)