add_executable(
    ${PROJECT_NAME}
    archiver.cc
    filereader.cc
    filewriter.cc
    huffman.cc
    main.cc
)

find_package(Boost REQUIRED)
target_link_libraries(${PROJECT_NAME} boost::boost)
