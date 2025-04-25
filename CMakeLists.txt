cmake_minimum_required(VERSION 3.21)

set(CMAKE_OSX_DEPLOYMENT_TARGET "15" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++20")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(bootstrap_vcpkg)

project(timelines)

set(HEADERS
        src/headers.hpp
        src/sdl/graphics.hpp
        src/core.hpp
        src/types.hpp
        src/app.hpp
        src/app.cpp
        src/sdl/graphics.cpp
        src/backend.hpp
        src/time.cpp
        src/files.hpp
        src/ui/base.hpp)

# Sources
aux_source_directory(src SRC_LIST)
aux_source_directory(src/sdl DETAILS_SRC_LIST)

add_compile_definitions(USE_SDL=1)
add_compile_definitions(CMAKE_ROOT=\"${CMAKE_CURRENT_SOURCE_DIR}\")
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
        turuluu::utlz
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${vcpkg_installed_dir}/include
        deps
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
target_include_directories(
        ${PROJECT_NAME}_lib
        PUBLIC
        ${vcpkg_installed_dir}/include
        deps
)
target_link_libraries(${PROJECT_NAME}_lib ${dependencies})
add_subdirectory(tests)