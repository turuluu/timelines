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

Building
-----------------------------------------------------

### Linux / MacOS

* example using ninja (for make, just drop the `-GNinja` flag and build with `make`)

    cmake -S . -B build -G Ninja
    cmake --build build

### Windows    

    # CMakeLists.txt can now be used as a project in CLion or build manually
    cmake -S . -B build -G 'Visual Studio 17 2022'

    # Open build/timelines.sln in Visual Studio

Running
-------

    ./build/timelines

License
-----------------------------------------------

Licenses are grouped under directories. The child level LICENSE file overrides the parent level equivalent, for example, LICENSE at the root of this repo applies to the code inside the root directory of the repo and other child directories where an explicit LICENSE file is not available - when the LICENSE file under vera-fonts directory applies to the files inside said directory like the dejavusans ttffile.

Metrics
-------

    scc --by-file -s complexity -a -u -i cpp,hpp
