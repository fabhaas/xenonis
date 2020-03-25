# bigint - a C++ bigint implementation
(C) 2018-2020 Fabian Haas

## Installation

### Requirements - all plattforms

- conan (https://conan.io/)

- cmake (https://cmake.org/)

- cmake supported generator (https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)


### Building - all plattforms

A basic script to configure and build the library looks like this:

```bash
git clone https://gitlab.com/fahaas/bigint.git #clone repo
cd bigint
sh setup.sh # or setup.bat
mkdir build #create build directory
conan install .. #install libraries, internet access required
cmake .. -G <generator> <options> #generate build files
ninja #when using ninja as generator; build
```

### Building on Linux - detailed version

#### Ubuntu

Install the required packages using this command:

```bash
sudo apt-get install cmake build-essential python-pip
sudo pip install conan #install conan
```

Then to configure and build the library run this:

```bash
git clone https://gitlab.com/fahaas/bigint.git #clone repo
cd bigint
sh setup.sh
mkdir build #create build directory
conan install .. #install libraries, internet access required
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release #generate build files in release mode
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
git clone https://gitlab.com/fahaas/bigint.git #clone repo
cd bigint
sh setup.sh
mkdir build #create build directory
conan install .. #install libraries, internet access required
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release #generate build files in release mode
make
```

## TODO
+ check for support for adcx, adox and mulx instruction
