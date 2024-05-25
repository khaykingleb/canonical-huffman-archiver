#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

/**
 * A class for writing to a file bit by bit.
 */
class FileWriter
{
public:
    /**
     * Constructor.
     * @param file_path The path to the file to write to.
     */
    explicit FileWriter(const std::string& file_path);

    /**
     * Destructor.
     */
    ~FileWriter();

    /**
     * Write a character to the file.
     * @param character The character to write.
     */
    void WriteCharacter(unsigned char character);

    /**
     * Write an integer encoded with variable-length bit encoding.
     * @param number The integer to write.
     * @param num_bits The number of bits to write.
     */
    void WriteHuffmanInt(uint64_t number, size_t num_bits = 9);

    /**
     * Write a Huffman code to the file.
     * @param huffman_code The Huffman code to write.
     */
    void WriteHuffmanCode(const std::string& huffman_code);

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
    std::string file_path_;
    std::ofstream file_;

    unsigned char buffer_byte_ { 0 };
    uint8_t bit_pos_ { 0 };

    size_t file_size_ { 0 };
};
