#pragma once

#include <fstream>

class FileWriter
{
public:
    explicit FileWriter(const std::string& archive_path);
    ~FileWriter();

    // FileWriter(const FileWriter& file_writer) = delete;
    // FileWriter& operator=(const FileWriter& file_writer) = delete;
    // FileWriter(FileWriter&& file_writer) = default;
    // FileWriter& operator=(FileWriter&& file_writer) = default

    void WriteBits(unsigned char c, size_t bits_count);

    // void WriteByte(unsigned char byte);
    // void WriteBit(bool bit);

protected:
    void Flush();

private:
    std::string archive_path_;
    std::ofstream file_;

    unsigned char buffer_byte_ { 0 };
    uint8_t bits_pos_ { 0 };

    size_t file_size_ { 0 };

    // std::bitset<8> _buffer;
    // size_t file_size_ { 0 };
    // size_t bytes_written_ { 0 };
};
