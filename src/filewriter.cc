#include "filewriter.h"

#include <cstddef>
#include <cstdint>
#include <vector>

FileWriter::FileWriter(const std::string& archive_path)
    : archive_path_(archive_path)
    , file_(archive_path_, std::ofstream::binary)

{
    if (!file_.is_open())
    {
        throw std::runtime_error("The file writer cannot open " + archive_path_);
    }
}

FileWriter::~FileWriter()
{
    if (file_.is_open())
    {
        FlushBuffer();
        file_.close();
    }
}

void FileWriter::WriteHuffmanInt(uint64_t number, size_t num_bits)
{
    std::vector<bool> bits(num_bits);
    for (size_t i = 0; i < num_bits; ++i)
    {
        // Fill from the most significant bit to the least significant bit
        bits[num_bits - 1 - i] = (number >> i) & 1;
    }

    WriteBits(bits);
}

void FileWriter::WriteBits(const std::vector<bool>& bits)
{
    for (bool bit : bits)
    {
        // Add bit to buffer
        if (bit)
        {
            // Set specific bit in the buffer byte without altering other bits that are already set
            buffer_byte_ |= (1 << (7 - bit_pos_));
        }

        ++bit_pos_;

        // If buffer is full, write it out to the file
        if (bit_pos_ == 8)
        {
            FlushBuffer();
        }
    }
}

void FileWriter::FlushBuffer()
{
    if (bit_pos_ > 0)
    {
        file_.put(static_cast<char>(buffer_byte_));

        buffer_byte_ = 0;
        bit_pos_ = 0;

        ++file_size_;
    }
}
