#include "filereader.h"
#include "filewriter.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <sys/types.h>
#include <vector>

TEST(FileReaderTest, ConstructWithValidPath)
{
    ASSERT_NO_THROW(FileReader reader("test.txt"));
}

TEST(FileReaderTest, ConstructWithInvalidPath)
{
    ASSERT_THROW(FileReader reader("invalid_path.txt"), std::runtime_error);
}

TEST(FileReaderTest, GetFileName)
{
    FileReader reader("test.txt");
    ASSERT_EQ(reader.GetFileName(), "test.txt");
}

TEST(FileReaderTest, HasMoreCharacters)
{
    FileReader reader("test.txt");
    ASSERT_TRUE(reader.HasMoreCharacters());
}

TEST(FileReaderTest, ReadCharacter)
{
    FileReader reader("test.txt");
    ASSERT_EQ(reader.ReadCharacter().value(), 'N');
}

TEST(FileTest, ReadHuffmanInt)
{
    std::vector<uint16_t> corner_nine_bit_numbers = { 0, 511 };

    {
        FileWriter writer("test_huffman.txt");
        for (const auto& number : corner_nine_bit_numbers)
        {
            writer.WriteHuffmanInt(number);
        }
    } // ensuring FileWriter is destroyed and file is closed here

    FileReader reader("test_huffman.txt");
    ASSERT_EQ(reader.ReadHuffmanInt(), corner_nine_bit_numbers[0]);
    ASSERT_EQ(reader.ReadHuffmanInt(), corner_nine_bit_numbers[1]);
}

TEST(FileTest, WriteHuffmanCode)
{
    std::string huffman_code = "101010101010101010101010101010101010101010101010";
    size_t number_of_bits = huffman_code.size();
    uint64_t huffman_code_int = std::stoll(huffman_code, nullptr, 2);

    {
        FileWriter writer("test_huffman_code.txt");
        writer.WriteHuffmanInt(huffman_code_int, number_of_bits);
    } // ensuring FileWriter is destroyed and file is closed here

    FileReader reader("test_huffman_code.txt");
    ASSERT_EQ(reader.ReadHuffmanInt(number_of_bits), huffman_code_int);
}

TEST(FileReaderTest, ResetPositionToStart)
{
    FileReader reader("test.txt");

    auto character = reader.ReadCharacter();
    ASSERT_EQ(character.value(), 'N');

    reader.ResetPositionToStart();
    auto character_after_reset = reader.ReadCharacter();
    ASSERT_EQ(character_after_reset.value(), 'N');
}
