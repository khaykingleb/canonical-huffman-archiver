#include "archiver.h"

#include "filereader.h"
#include "filewriter.h"
#include "huffman.h"

Archiver::Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths)
    : archive_path_(archive_path), file_paths_(file_paths)
{
}

Archiver::Archiver(const std::string& archive_path) : archive_path_(archive_path) { }

void Archiver::Compress() const
{
    FileWriter writer(archive_path_);
    for (const auto& file_path : file_paths_)
    {
        HuffmanCoder huffman_coder;
        FileReader reader(file_path);
        bool is_last_file = file_path == file_paths_.back();
        huffman_coder.Encode(reader, writer, is_last_file);
    }
}

void Archiver::Decompress() const
{
    FileReader reader(archive_path_);
    bool need_to_decode_next_file = true;
    while (need_to_decode_next_file)
    {
        HuffmanCoder huffman_coder;
        need_to_decode_next_file = huffman_coder.Decode(reader);
    }
}
