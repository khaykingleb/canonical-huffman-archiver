#include "archiver.h"

#include "filereader.h"
#include "filewriter.h"
#include "huffman.h"

#include <cstddef>

Archiver::Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths)
    : archive_path_(archive_path), file_paths_(file_paths)
{
}

Archiver::Archiver(const std::string& archive_path) : archive_path_(archive_path) { }

void Archiver::Compress()
{
    FileWriter writer(archive_path_);

    for (size_t i = 0; i < file_paths_.size(); ++i)
    {
        FileReader reader(file_paths_[i]);

        HuffmanCoder huffman_coder(reader, writer);
        huffman_coder.Encode();

        // Write the end of the file
        writer.WriteHuffmanInt(FILENAME_END);

        if (i < file_paths_.size() - 1)
        {
            // Write the start of the next file in the archive if there is one
            writer.WriteHuffmanInt(ONE_MORE_FILE);
        }
        else
        {
            // Write the end of the archive if there are no more files to write
            writer.WriteHuffmanInt(ARCHIVE_END);
        }
    }
}
