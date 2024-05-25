#pragma once

#include "filereader.h"
#include "trie.h"

#include <string>
#include <vector>

/**
 * A class for archiving and unarchiving files.
 */
class Archiver
{
public:
    /**
     * Constructor.
     * @param archive_path The path to the archive file.
     * @param file_paths The paths to the files to archive.
     */
    Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths);

    /**
     * Constructor.
     * @param archive_path The path to the archive file.
     */
    Archiver(const std::string& archive_path);

    /**
     * Compress the files to the archive.
     */
    void Compress();

    /**
     * Decompress the archive to get the files.
     */
    void Decompress();

protected:
    /**
     * Get the symbols from the encoded file.
     * @param symbols_count The number of symbols to read.
     * @param reader The file reader to read the symbols from the encoded file.
     * @return The symbols read from the file.
     */
    Symbols RestoreSymbols(uint16_t symbols_count, FileReader& reader);

    /**
     * Get the symbol counts with the same code lengths from the encoded file.
     * @param symbols The symbols to get the codes for.
     * @param reader The file reader to read the counts from the encoded file.
     * @return The counts of symbols with the same code lengths.
     */
    std::vector<size_t> RestoreSymbolsCountsWithSameCodeLengths(Symbols symbols,
                                                                FileReader& reader);

    /**
     * Restore the file name from the encoded file.
     * @param reader The file reader to read the file name from the encoded file.
     * @param trie The trie to use for decoding the file name.
     * @return The file name.
     */
    std::string RestoreFileName(FileReader& reader, BinaryTrie& trie);

private:
    std::string archive_path_;
    std::vector<std::string> file_paths_;
};
