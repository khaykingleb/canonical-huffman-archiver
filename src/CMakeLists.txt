add_library(
    ${PROJECT_NAME}_lib
    filereader.cc
    filewriter.cc
    huffman.cc
    archiver.cc
    trie.cc
)
find_package(Boost REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}_lib
    boost::boost
)

add_executable(
    ${PROJECT_NAME}
    main.cc
)
target_link_libraries(
    ${PROJECT_NAME}
    ${PROJECT_NAME}_lib
)
