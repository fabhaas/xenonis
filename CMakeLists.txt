# ---------- (C) 2018 fahaas ----------
cmake_minimum_required (VERSION 3.10)

project (xenonis
    VERSION 0.0.5 #is based on older projects
)

#include modules
include (CTest)

#options
option (XENONIS_USE_OPENMP "Use OpenMP for hardware acceleration" ON)
option (XENONIS_USE_UINT128 "Use __int128 extension" ON)

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
endif ()

#conan configuration
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_add_remote(NAME bincrafters INDEX 1
            URL https://api.bintray.com/conan/bincrafters/public-conan)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP
                BUILD missing)

#include libraries
include_directories ("${CMAKE_BINARY_DIR}")

#add subdirectories
add_subdirectory (src)
add_subdirectory (test)
add_subdirectory (bench)
