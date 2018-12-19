#(C) 2018 fahaas
add_library(
	bigint
	bigint.cpp
)

target_link_libraries(
	bigint
	${CONAN_LIBS}
)
