cmake_minimum_required(VERSION 3.21)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(timelines)

aux_source_directory(src SRC_LIST)
aux_source_directory(src/details DETAILS_SRC_LIST)

set(HEADERS
        src/headers.hpp
        src/details/csv/csv.h
        src/details/graphics.hpp
        src/details/graphics.hpp
        src/resource_mgr.hpp)

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