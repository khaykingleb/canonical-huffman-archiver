#include "archiver.h"
#include "filereader.h"

#include <gtest/gtest.h>

TEST(ArchiverTest, CompressionAndDecompressionOfOneFile)
{
    std::string original_file_text;
    {
        FileReader reader("test_1.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                original_file_text.push_back(character.value());
            }
        }
    }

    std::string archive_path = "test_archive.huff";
    std::vector<std::string> file_paths = { "test_1.txt" };
    Archiver archiver(archive_path, file_paths);

    archiver.Compress();
    archiver.Decompress();

    std::string decompressed_file_text;
    {
        FileReader reader("test_1.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                decompressed_file_text.push_back(character.value());
            }
        }
    }

    EXPECT_EQ(original_file_text, decompressed_file_text);
}

TEST(ArchiverTest, CompressionAndDecompressionOfSeveralFiles)
{
    std::string original_file_text_1;
    {
        FileReader reader("test_1.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                original_file_text_1.push_back(character.value());
            }
        }
    }

    std::string original_file_text_2;
    {
        FileReader reader("test_2.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                original_file_text_2.push_back(character.value());
            }
        }
    }

    std::string archive_path = "test_archive.huff";
    std::vector<std::string> file_paths = { "test_1.txt", "test_2.txt" };
    Archiver archiver(archive_path, file_paths);

    archiver.Compress();
    archiver.Decompress();

    std::string decompressed_file_text;
    {
        FileReader reader("test_1.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                decompressed_file_text.push_back(character.value());
            }
        }
    }

    EXPECT_EQ(original_file_text_1, decompressed_file_text);

    std::string decompressed_file_text_2;
    {
        FileReader reader("test_2.txt");
        while (reader.HasMoreCharacters())
        {
            auto character = reader.ReadCharacter();
            if (character.has_value())
            {
                decompressed_file_text_2.push_back(character.value());
            }
        }
    }

    EXPECT_EQ(original_file_text_2, decompressed_file_text_2);
}
