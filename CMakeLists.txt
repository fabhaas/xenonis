#(C) 2018 fahaas
cmake_minimum_required(VERSION 3.10)

project(bigint 
    VERSION 0.0.5 #is based on older projects
)

#include modules
include(CTest)

#CXX configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fopenmp -DNDEBUG -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fopenmp")

#enable colored output when using ninja
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdiagnostics-color=always")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fcolor-diagnostics")
endif()

#conan configuration
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#add subdirectories
add_subdirectory(src)
add_subdirectory(test)