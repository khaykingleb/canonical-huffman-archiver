#include "filereader.h"
#include <filesystem>
#include <iostream>

FileReader::FileReader(const std::string& file_path)
    : file_path_(file_path)
    , file_(file_path_, std::ifstream::binary)
{
    if (!file_.is_open())
    {
        throw std::runtime_error("The file reader cannot open " + file_path_);
    }

    // Set the file pointer to the end of the file to determine the file size
    file_.seekg(0, std::ios::end);
    file_size_ = file_.tellg();

    // Reset the file pointer to the beginning
    file_.seekg(0, std::ios::beg);
}

FileReader::~FileReader()
{
    if (file_.is_open())
    {
        file_.close();
    }
}

bool FileReader::HasMoreCharacters() const
{
    return !file_.eof();
}

std::optional<unsigned char> FileReader::ReadCharacter()
{
    char character;
    if (file_.get(character))
    {
        return static_cast<unsigned char>(character);
    }

    return std::nullopt;
}

std::string FileReader::GetFileName() const
{
    return std::filesystem::path(file_path_).filename().string();
}

std::vector<bool> FileReader::ReadBits(size_t bits_count)
{
    std::vector<bool> bits;
    for (size_t i = 0; i < bits_count; ++i)
    {
        if (file_.eof())
        {
            break;
        }

        bits.emplace_back(buffer_byte_ & (1 << bits_pos_));
        ++bits_pos_;

        if (bits_pos_ == 8)
        {
            char character;
            file_.read(&character, 1);
            buffer_byte_ = static_cast<unsigned char>(character);
        }
    }

    return bits;
}

uint64_t FileReader::ReadHuffmanInt(size_t bits_count)
{
    auto bits = ReadBits(bits_count);

    uint64_t value = 0;
    for (size_t i = 0; i < bits.size(); ++i)
    {
        value |= bits[i] << i;
    }

    return value;
}
