set(TARGET_NAME ${PROJECT_NAME}_tests)
add_executable(${TARGET_NAME}
        timelines_test.cpp
        doctest_main.cpp)

target_link_libraries(${TARGET_NAME}
        doctest::doctest
        ${PROJECT_NAME}_lib)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
