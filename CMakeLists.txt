# ---------- (C) 2018, 2019 fahaas ----------
cmake_minimum_required (VERSION 3.10)

project (xenonis
    VERSION 0.0.7 #is based on older projects
)

#include modules
include (CTest)

#options
option (XENONIS_USE_OPENMP "Use OpenMP for hardware acceleration" ON)
option (XENONIS_USE_UINT128 "Use __int128 extension" ON)
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    option(XENONIS_BUILD_DOC "Build documentation" ON)
else()
    option(XENONIS_BUILD_DOC "Build documentation" OFF)
endif()
set (XENONIS_REQUIRED_LIBS ${CMAKE_SOURCE_DIR}/build/asm/add.o)

#CXX configuration
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-rtti -DNDEBUG")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17") #-masm=intel

if (XENONIS_USE_OPENMP)
    #find_package(OpenMP REQUIRED)
    #add_compile_options (-fopenmp)
    #set (XENONIS_REQUIRED_LIBS ${XENONIS_REQUIRED_LIBS} omp)
    add_compile_options (${OpenMP_CXX_FLAGS})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (XENONIS_REQUIRED_LIBS ${XENONIS_REQUIRED_LIBS} ${OpenMP_CXX_LIBRARIES})
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
#conan_add_remote(NAME bincrafters INDEX 1
#	URL https://api.bintray.com/conan/bincrafters/public-conan)
#conan_add_remote(NAME mpusz
#	    URL https://api.bintray.com/conan/mpusz/conan-mpusz)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP
    BUILD missing)

# check if Doxygen is installed
if (XENONIS_BUILD_DOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(XENONIS_DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
        set(XENONIS_DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${XENONIS_DOXYGEN_IN} ${XENONIS_DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        add_custom_target( doc_doxygen ALL
                           COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                            COMMENT "Generating documentation with Doxygen"
                            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif (XENONIS_BUILD_DOC)

#include libraries
include_directories ("${CMAKE_BINARY_DIR}")

#add subdirectories
add_subdirectory (include)
add_subdirectory (test)
add_subdirectory (bench)
