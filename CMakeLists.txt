# ---------- (C) 2018 fahaas ----------
cmake_minimum_required (VERSION 3.10)

project (bigint
    VERSION 0.0.5 #is based on older projects
)

#include modules
include (CTest)

#options
option (USE_OPENMP "Use OpenMP for hardware acceleration" ON)

#CXX configuration
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")

if (USE_OPENMP)
    add_compile_options ("-fopenmp")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options ("-fdiagnostics-color=always") #enable colored output when using ninja
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options ("-fcolor-diagnostics") #enable colored output when using ninja
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    option (USE_UINT128 "Use __int128 extension" ON)
else ()
    option (USE_UINT128 "Use __int128 extension" OFF)
endif ()

#conan configuration
include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories ("${CMAKE_BINARY_DIR}")

#add subdirectories
add_subdirectory (src)
add_subdirectory (test)
