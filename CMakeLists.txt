#(C) 2018 fahaas
cmake_minimum_required(VERSION 3.10)

project(bigint 
    VERSION 0.0.5 #is based on older projects
)

#CXX configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#conan configuration
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#add subdirectories
add_subdirectory(src)
add_subdirectory(test)