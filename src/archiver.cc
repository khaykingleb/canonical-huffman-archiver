#include "archiver.h"
#include "filereader.h"
#include "filewriter.h"
#include "huffman.h"

#include <cstddef>
#include <iostream>
#include <ranges>


Archiver::Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths)
    : archive_path_(archive_path)
    , file_paths_(file_paths)
{
}

Archiver::Archiver(const std::string& archive_path)
    : archive_path_(archive_path)
{
}


void Archiver::Compress()
{
    FileWriter writer(archive_path_);

    for (size_t i = 0; i < file_paths_.size(); ++i)
    {
        FileReader reader(file_paths_[i]);

        HuffmanCoder huffman_coder(writer, reader);
        huffman_coder.BuildCanonicalCodes(); // преместить в huffman_coder.Encode()
        huffman_coder.Encode();
    }
}
