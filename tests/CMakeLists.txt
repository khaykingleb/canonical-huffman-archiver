enable_testing()
find_package(GTest REQUIRED)

file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_1.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_2.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

function(add_gtest testname)
    add_executable(
        ${testname}
        ${testname}.cc
    )
    target_include_directories(
        ${testname}
        PRIVATE ../src
    )
    target_link_libraries(
        ${testname}
        gtest::gtest
        ${PROJECT_NAME}_lib
    )
    add_test(
        NAME ${testname}
        COMMAND ${testname}
    )
endfunction()

add_gtest(test_archiver)
add_gtest(test_file)
add_gtest(test_huffman)
