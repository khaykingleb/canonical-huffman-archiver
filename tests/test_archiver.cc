#include "archiver.h"
#include "filereader.h"

#include <gtest/gtest.h>

TEST(ArchiverTest, CompressionAndDecompression)
{
    std::string original_file_text;
    {
        FileReader reader("test.txt");
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
    std::vector<std::string> file_paths = { "test.txt" };
    Archiver archiver(archive_path, file_paths);

    archiver.Compress();
    archiver.Decompress();

    std::string decompressed_file_text;
    {
        FileReader reader("test.txt");
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
