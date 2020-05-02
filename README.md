# xenonis - a C++17 bigint implementation
(C) 2018-2020 Fabian Haas

xenonis is a portable header-only C++17 library which implements a basic bigint class which supports addition, subtraction and multiplication. bigint can be constructed using integers and hex-strings.

The library implements the naive addition, subtraction and multiplication using clean C++17 and
additionally using x86_64 assembly. The [Karatsuba](https://en.wikipedia.org/wiki/Karatsuba_algorithm) algorithm is implemented using only C++17. All 64-bit plattforms supported by Clang or GCC can be used.

NOTE: the x86_64 assembly will be enabled automatically at compile-time (when on x86_64 using GCC or Clang). The code assumes that the `adox`, `adcx` instructions are supported. Please ensure the availability or disable the use of assembly by passing `-DXENONIS_USE_INLINE_ASM=OFF` to cmake.

Example:
```cpp
#include <bigint/bigint.hpp>
#include <iostream>

int main()
{
    xenonis::bigint a("ffffffffffffffffff");
    xenonis::bigint b("ffffffffffffffffff");

    // print the result: 
    std::cout << a * b << '\n';
    
    // print length of the hex-string:
    std::cout << (a * b).to_string().size() << '\n';
    return 0;
}
```

## Installation

### Requirements - all plattforms

- cmake (https://cmake.org/)

- cmake supported generator (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

- OPTIONAL(to build the tests and benchmarks): conan (https://conan.io/)

- OPTIONAL(to build the tests and benchmarks): the GNU MP library (https://gmplib.org)

### Installation
```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd bigint
cmake .. -G <generator>
make install #when using make
```

### Building the tests and benchmarks - all plattforms

A basic script to configure and build the library looks like this:

```bash
git clone https://github.com/fabhaas/xenonis.git #clone repo
cd bigint
sh setup.sh # or setup.bat
mkdir build #create build directory
conan install .. #install libraries, internet access required
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
cd bigint
sh setup.sh
mkdir build #create build directory
conan install .. #install libraries, internet access required
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DXENONIS_BUILD_TESTS=ON \
    -DXENONIS_BUILD_BENCHMARKS=ON #generate build files in release mode
make
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
cd bigint
sh setup.sh
mkdir build #create build directory
conan install .. #install libraries, internet access required
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DXENONIS_BUILD_TESTS=ON \
    -DXENONIS_BUILD_BENCHMARKS=ON #generate build files in release mode
make
```

## TODO
+ check for support for adcx, adox and mulx instruction
