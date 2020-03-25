clang-format -style=file -i include/*.hpp include/algorithms/*.hpp test/*.cpp bench/*.cpp
cmake-format -i include/CMakeLists.txt test/CMakeLists.txt bench/CMakeLists.txt ./CMakeLists.txt
