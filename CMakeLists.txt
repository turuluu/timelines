cmake_minimum_required(VERSION 3.21)

set(CMAKE_OSX_DEPLOYMENT_TARGET "15" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if (WIN32)
        set(VCPKG_TARGET_TRIPLET x64-windows)
    else ()
        if (FORCE_INTEL)
            set(CMAKE_OSX_DEPLOYMENT_TARGET 15.0)
            set(VCPKG_TARGET_ARCHITECTURE "x64")
            set(VCPKG_TARGET_TRIPLET x64-osx)
        else ()
            set(CMAKE_OSX_ARCHITECTURES arm64)
            set(VCPKG_TARGET_ARCHITECTURE "arm64")
            set(VCPKG_TARGET_TRIPLET arm64-osx)
        endif ()
        set(triplet_dir ${VCPKG_TARGET_TRIPLET})
        set(VCPKG_HOST_TRIPLET ${VCPKG_TARGET_TRIPLET})
    endif ()
    # Set up a local installation with vcpkg for the dependencies
    # starting with cloning the vcpkg with the git submodule setup (cmake handles cloning
    # if not found) and proceeding with bootstrapping it.
    set(local_vcpkg "${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg")
    # Sets up shop so that all dependencies can be found at .../include or .../lib
    set(VCPKG_INSTALLED_DIR "${local_vcpkg}_installed")
    set(vcpkg_platform_dir ${VCPKG_INSTALLED_DIR})
    set(vcpkg_installed_deps ${vcpkg_platform_dir}/${triplet_dir})
    # Bootstrap vcpkg
    set(vcpkg_cmake_script "${local_vcpkg}/scripts/buildsystems/vcpkg.cmake")
    # Setup cloning script path
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    # Clone the vcpkg submodule if not found and set cmake_toolchain
    # This should propagate "manifest" mode for vcpkg and further installation of dependencies
    set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    include(vcpkg)
endif ()

project(timelines)

set(HEADERS
        src/headers.hpp
        src/sdl/graphics.hpp
        src/core.hpp
        src/types.hpp
        src/app.hpp
        src/app.cpp
        src/sdl/graphics.cpp)

# Sources
aux_source_directory(src SRC_LIST)
aux_source_directory(src/sdl DETAILS_SRC_LIST)

add_compile_definitions(USE_SDL=1)
add_executable(${PROJECT_NAME}
        ${SRC_LIST}
        ${DETAILS_SRC_LIST}
)
add_subdirectory(deps/utlz)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

set(dependencies
        Freetype::Freetype
        BZip2::BZip2
        ZLIB::ZLIB
        cpr::cpr
        nlohmann_json::nlohmann_json
        SDL3_ttf::SDL3_ttf
        SDL3::SDL3
        utlz
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${vcpkg_installed_dir}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/deps
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