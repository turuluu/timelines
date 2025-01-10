cmake_minimum_required(VERSION 3.21)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# In some corner cases on Windows, this might not work: https://gitlab.kitware.com/cmake/cmake/-/issues/18862
if ((CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR))
    if (WIN32)
        set(VCPKG_TARGET_TRIPLET x64-windows)
    else ()
        set(CMAKE_OSX_ARCHITECTURES arm64)
        set(VCPKG_TARGET_ARCHITECTURE "arm64")
        set(VCPKG_TARGET_TRIPLET arm64-osx)

        set(platform_DIR ${VCPKG_TARGET_TRIPLET})
        set(VCPKG_HOST_TRIPLET ${VCPKG_TARGET_TRIPLET})
    endif ()

    # Set up a local installation with vcpkg for the dependencies
    # starting with cloning the vcpkg with the git submodule setup (cmake handles clonging
    # if not found) and proceeding with bootstrapping it.
    set(local_vcpkg "${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg")
    # Sets up shop so that all dependencies can be found at .../include or .../lib
    set(VCPKG_INSTALLED_DIR "${local_vcpkg}_installed")
    set(roti_vcpkg_platform_dir ${VCPKG_INSTALLED_DIR})
    # Bootstrap vcpkg
    set(vcpkg_cmake_script "${local_vcpkg}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    # Clone the vcpkg submodule if not found and set cmake_toolchain
    # This should propagate "manifest" mode for vcpkg and further installation of dependencies
    set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    # from cmake/
    include(vcpkg)

endif ()

project(timelines)

aux_source_directory(src SRC_LIST)
aux_source_directory(src/sdl DETAILS_SRC_LIST)

set(HEADERS
        src/headers.hpp
        src/sdl/graphics.hpp
        src/core.hpp)

add_executable(${PROJECT_NAME}
        ${SRC_LIST}
        ${DETAILS_SRC_LIST}
        ${HEADERS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
#set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
find_package(doctest CONFIG REQUIRED)

set(dependencies
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        BZip2::BZip2
        ZLIB::ZLIB
        cpr::cpr
        nlohmann_json::nlohmann_json
)

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${dependencies}
)

add_library(${PROJECT_NAME}_lib
        ${SRC_LIST}
        ${DETAILS_SRC_LIST}
        ${HEADERS}
)
target_link_libraries(${PROJECT_NAME}_lib ${dependencies})
add_subdirectory(tests)