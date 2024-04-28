#pragma once

#include <fstream>
#include <optional>
#include <sys/types.h>
#include <vector>

/**
 * FileReader class is responsible for reading bytes and bits from a file.
 * It provides methods to read the next byte or bit from the file.
 */
class FileReader
{
public:
    /**
     * Constructs a FileReader object with the given file path.
     *
     * @param file_path The path to the file to read from.
     */
    explicit FileReader(const std::string& file_path);

    ~FileReader();

    bool HasMoreCharacters() const;

    std::optional<unsigned char> ReadCharacter();


    std::string GetFileName() const;

    uint64_t ReadHuffmanInt(size_t bits_count);

protected:
    std::vector<bool> ReadBits(size_t bits_count);

    // size_t GetFileSize() const;

    // bool HasNextByte() const;
    // unsigned char ReadNextByte();

    // bool HasNextBit() const;
    // bool ReadNextBit();

private:
    std::string file_path_;
    std::ifstream file_;

    unsigned char buffer_byte_ { 0 };
    uint8_t bits_pos_ { 0 };

    size_t file_size_ { 0 };
    size_t bytes_read_ { 0 };
};
