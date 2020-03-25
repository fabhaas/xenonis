# ---------- (C) 2018-2020 Fabian Haas ----------
cmake_minimum_required(VERSION 3.10)

project(
  xenonis
  VERSION 0.1.0
  LANGUAGES CXX) # is based on older projects

# include modules
include(CTest)

# options
option(XENONIS_BUILD_TESTS "Build the tests" OFF)
option(XENONIS_BUILD_BENCHMARKS "Build the benchmarks" OFF)
option(XENONIS_USE_UINT128 "Use __int128 extension" ON)
option(XENONIS_USE_INLINE_ASM "Use inline assembly" ON)
if(CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
  option(XENONIS_BUILD_DOC "Build documentation" ON)
else()
  option(XENONIS_BUILD_DOC "Build documentation" OFF)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options("-fdiagnostics-color=always") # enable colored output when
                                                    # using ninja
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options("-fcolor-diagnostics") # enable colored output when using
                                             # ninja
endif()

if(XENONIS_BUILD_TESTS OR XENONIS_BUILD_BENCHMARKS)
  # conan configuration
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(
      STATUS
        "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
      "${CMAKE_BINARY_DIR}/conan.cmake")
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)
  conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP BUILD missing)
endif()

# check if Doxygen is installed
if(XENONIS_BUILD_DOC)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    # set input and output files
    set(XENONIS_DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
    set(XENONIS_DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${XENONIS_DOXYGEN_IN} ${XENONIS_DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target(
      doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating documentation with Doxygen"
      VERBATIM)
  else(DOXYGEN_FOUND)
    message(
      "Doxygen need to be installed to generate the doxygen documentation")
  endif(DOXYGEN_FOUND)
endif(XENONIS_BUILD_DOC)

# include libraries
include_directories("${CMAKE_BINARY_DIR}")

# add subdirectories
add_subdirectory(include)

if(XENONIS_BUILD_TESTS)
  add_subdirectory(test)
endif()

if(XENONIS_BUILD_BENCHMARKS)
  add_subdirectory(bench)
endif()
