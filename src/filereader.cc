#include "filereader.h"

#include <filesystem>

FileReader::FileReader(const std::string& file_path)
    : file_path_(file_path), file_(file_path_, std::ifstream::binary)
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

std::string FileReader::GetFileName() const
{
    return std::filesystem::path(file_path_).filename().string();
}

void FileReader::ResetPositionToStart()
{
    file_.clear();
    file_.seekg(0, std::ios::beg);

    buffer_byte_ = 0;
    bit_pos_ = 0;
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

uint64_t FileReader::ReadHuffmanInt(size_t num_bits)
{
    uint64_t number = 0;
    for (size_t i = 0; i < num_bits; ++i)
    {
        bool bit = ReadBit();
        // Shift the bit to the correct position starting from LSB
        number |= static_cast<uint64_t>(bit) << i;
    }
    return number;
}

bool FileReader::ReadBit()
{
    // If the buffer is empty, read the next byte
    if (bit_pos_ == 0)
    {
        char byte;
        if (!file_.get(byte))
        {
            throw std::runtime_error("Failed to read a byte from the file");
        }

        buffer_byte_ = byte;
    }

    // Read the bit starting from LSB
    bool bit = buffer_byte_ & (1 << bit_pos_);

    ++bit_pos_;
    bit_pos_ %= 8;

    return bit;
}
