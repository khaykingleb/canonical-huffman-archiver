#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>

/**
 * A class for writing to a file bit by bit.
 */
class FileWriter
{
public:
    /**
     * Constructor.
     * @param archive_path The path to the file to write to.
     */
    explicit FileWriter(const std::string& archive_path);

    /**
     * Destructor.
     */
    ~FileWriter();

    /**
     * Write an integer encoded with variable-length bit encoding.
     * @param number The integer to write.
     * @param num_bits The number of bits to write.
     */
    void WriteHuffmanInt(uint64_t number, size_t num_bits = 9);

protected:
    /**
     * Write a bit to the file.
     * @param bit The bit to write.
     */
    void WriteBits(const std::vector<bool>& bits);

    /**
     * Flush the byte buffer to the file.
     */
    void FlushBuffer();

private:
    std::string archive_path_;
    std::ofstream file_;

    unsigned char buffer_byte_ { 0 };
    uint8_t bit_pos_ { 0 };

    size_t file_size_ { 0 };
};
