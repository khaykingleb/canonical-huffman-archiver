#include "filewriter.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

FileWriter::FileWriter(const std::string& file_path)
    : file_path_(file_path), file_(file_path, std::ofstream::binary)
{
    if (!file_.is_open())
    {
        throw std::runtime_error("The file writer cannot open " + file_path);
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
        // Fill from the least significant bit to the most significant bit
        bits[i] = (number >> i) & 1;
    }

    WriteBits(bits);
}

void FileWriter::WriteHuffmanCode(const std::string& huffman_code)
{
    // Reverse code to write it from the least significant bit to the most significant bit
    std::string reversed_huffman_code(huffman_code.rbegin(), huffman_code.rend());
    uint64_t number = std::stoll(reversed_huffman_code, nullptr, 2);
    size_t num_bits = reversed_huffman_code.size();
    WriteHuffmanInt(number, num_bits);
}

void FileWriter::WriteBits(const std::vector<bool>& bits)
{
    for (bool bit : bits)
    {
        // Add bit to buffer
        if (bit)
        {
            // Set specific bit in the buffer byte without altering other bits that are already set
            buffer_byte_ |= (1 << bit_pos_);
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
