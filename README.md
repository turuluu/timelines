Timelines
=========

Present historical events and similar timeseries correlated factoids on a visual timeline.

WIP, MacOS, Linux and Windows

Requirements
------------

* vcpkg
* CMake
* Compiler that supports C++17 (recent version of g++, clang++ or Visual Studio >=2019)

Dependencies
-----------------------------------------------------

Dependencies handled by vcpkg. Generating build files using cmake also downloads the dependencies.

* SDL2
* [cpr](https://github.com/whoshuu/cpr)
* [doctest](https://github.com/onqtam/doctest/blob/master/doc/markdown/readme.md#reference)

One way of building with C++17, SDL2 and cmake
-----------------------------------------------------

### *Nix / MacOS

* example using ninja (for make, just drop the `-GNinja` flag and build with `make`)

    # CMakeLists.txt can now be used as a project in CLion or build manually
    cmake -GNinja ..
    ninja
    cd ..

### Windows    

    # CMakeLists.txt can now be used as a project in CLion or build manually
    cmake -G 'Visual Studio 16 2019' ..

    # Open build/main.sln and see additional notes for Windows below

Windows
-----------------------------------------------

- install scoop
- scoop install cmake conan
- git clone https://github.com/ben-strasser/fast-cpp-csv-parser to csv folder

Visual Studio 2019 does not respect cmake c++ standard definition (c++17), so..

change the language standard in Project properties -> Configuration Properties -> C/C++ -> Language -> C++ Language Standard to at least ISO C++17 Standard (/std:c++17)
(Can also be found in Project properties -> Configuration Properties -> General -> C++ Language Standard)
(Needs to be done to only the "main" under solution)

https://stackoverflow.com/questions/62256738/visual-studio-2019-c-and-stdfilesystem

Running
-------

    build/main

Cleaning up the binary file and build directory
-----------------------------------------------

    rm -rf build/ main

License
-----------------------------------------------

Licenses are grouped under directories. The child level LICENSE file overrides the parent level equivalent, for example, LICENSE at the root of this repo applies to the code inside the root directory of the repo and other child directories where an explicit LICENSE file is not available - when the LICENSE file under vera-fonts directory applies to the files inside said directory like the dejavusans ttffile.

Metrics
-------

    scc --by-file -s complexity -a -u -i cpp,hpp
