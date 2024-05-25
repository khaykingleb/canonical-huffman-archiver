#pragma once

#include <fstream>
#include <optional>

/*
 * A class for reading from a file byte by byte.
 */
class FileReader
{
public:
    /*
     * Constructor.
     * @param file_path The path to the file to read from.
     */
    explicit FileReader(const std::string& file_path);

    /*
     * Destructor.
     */
    ~FileReader();

    /*
     * Get the name of the file.
     * @return The name of the file.
     */
    std::string GetFileName() const;

    /*
     * Reset the file pointer to the beginning of the file.
     */
    void ResetPositionToStart();

    /*
     * Check if there are more characters to read from the file.
     * @return True if there are more characters to read, false otherwise.
     */
    bool HasMoreCharacters() const;

    /*
     * Read a character from the file.
     * @return The character read from the file, or std::nullopt if the end of the file is reached.
     */
    std::optional<unsigned char> ReadCharacter();

    /*
     * Read an integer encoded with variable-length bit encoding.
     * @param num_bits The number of bits to read.
     * @return The integer read from the file.
     */
    uint64_t ReadHuffmanInt(size_t num_bits = 9);

    /*
     * Read a bit from the file.
     * @return The bit read from the file.
     */
    bool ReadBit();

private:
    std::string file_path_;
    std::ifstream file_;

    unsigned char buffer_byte_ { 0 };
    uint8_t bit_pos_ { 0 };

    size_t file_size_ { 0 };
};
