# Timelines

Present historical events and similar timeseries correlated factoids on a visual timeline.

WIP, MacOS, Linux and Windows

## Usage

Compile and run with examples:

    ./build/timelines examples/example_prehistoric.csv

## Requirements

* CMake
* Compiler that supports C++20 (recent version of g++, clang++ or Visual Studio >=2019)

This should be handled by the cmake.
* vcpkg

## Dependencies

Dependencies handled by vcpkg. Generating build files using cmake also downloads the dependencies.

* SDL3
* [cpr](https://github.com/whoshuu/cpr)
* [doctest](https://github.com/onqtam/doctest/blob/master/doc/markdown/readme.md#reference)

## Building

CMakeLists.txt can be used as a project in, for example, CLion or Visual Studio.

Build manually by following the next examples.

### Linux / MacOS

Using ninja (for make, just drop the `-GNinja` flag)

    cmake -S . -B build -G Ninja
    cmake --build build

### Windows    

    cmake -S . -B build -G 'Visual Studio 17 2022'

=> Open build/timelines.sln in Visual Studio

## License

Licenses are grouped under directories. The child level LICENSE file overrides the parent level equivalent, for example, LICENSE at the root of this repo applies to the code inside the root directory of the repo and most child directories while the LICENSE file under vera-fonts directory explicitly applies to those files.

## Metrics

    scc --by-file -s complexity -a -u -i cpp,hpp
