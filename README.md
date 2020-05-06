# xenonis - a C++17 bigint implementation
xenonis is a portable header-only C++17 library which implements a basic bigint class which supports addition, subtraction and multiplication. bigint can be constructed using integers and hex-strings.

The library implements the naive addition, subtraction and multiplication using clean C++17 and additionally using x86_64 assembly. The [Karatsuba](https://en.wikipedia.org/wiki/Karatsuba_algorithm) algorithm is implemented using only C++17. All 64-bit platforms supported by Clang or GCC can be used.

NOTE: the x86_64 assembly will be enabled automatically at compile-time (when on x86_64 using GCC or Clang). The code assumes that the `adox`, `adcx` and `mulx` instructions are supported by the CPU. Please ensure the availability or disable the use of assembly by passing `-DXENONIS_USE_INLINE_ASM=OFF` to cmake.

Example:
```cpp
#include <bigint/bigint.hpp>
#include <iostream>

int main()
{
    xenonis::bigint a("ffffffffffffffffff");
    xenonis::bigint b("-ffffffffffffffffff"); // the hex-string may contain a -

    // print the result: 
    std::cout << a * b << '\n';
    
    // print length of the hex-string:
    std::cout << (a - b + a).to_string().size() << '\n';
    return 0;
}
```

## Installation

### Requirements - all platforms

- cmake (https://cmake.org/)

- cmake supported generator (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

- OPTIONAL(to build the documentation): doxygen (http://www.doxygen.nl/)

- OPTIONAL(to build the tests and benchmarks): conan (https://conan.io/)

### Installation
```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd xenonis
mkdir build && cd build #create build directory
cmake .. -G <generator>
make # make documentation
make install #when using make
```

### Building the tests and benchmarks - all platforms

A basic script to configure and build the library looks like this:

```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd xenonis
mkdir build && cd build #create build directory
cmake .. -G <generator> -DCMAKE_BUILD_TYPE=Release -DXENONIS_BUILD_TESTS=ON \
    -DXENONIS_BUILD_BENCHMARKS=ON #generate build files
ninja #when using ninja as generator; build
```

### Running the tests
```bash
./bin/bigint_test # has to be executed in the build directory
```

### Running the benchmarks
```bash
./bin/bigint_bench # has to be executed in the build directory
```

### Building the tests and benchmarks on Linux - detailed version

#### Ubuntu

Install the required packages using this command:

```bash
sudo apt-get install cmake build-essential python-pip
sudo pip install conan #install conan
```

Then to configure and build the library run this:

```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd xenonis
mkdir build && cd build #create build directory
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DXENONIS_BUILD_TESTS=ON \
    -DXENONIS_BUILD_BENCHMARKS=ON #generate build files in release mode
make # make documentation
make install # install
```

#### Fedora

Install the required packages using this command:

```bash
sudo dnf install cmake gcc-c++ make python-pip
sudo pip install conan #install conan
```

Then to configure and build the library run this:

```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd xenonis
mkdir build && cd build #create build directory
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DXENONIS_BUILD_TESTS=ON \
    -DXENONIS_BUILD_BENCHMARKS=ON #generate build files in release mode
make # make documentation
make install # install
```

## TODO
+ check for support for adcx, adox and mulx instruction

## License
The library is licensed under the Mozilla Public License 2.0 (MPL-2.0). See LICENSE for more information.
