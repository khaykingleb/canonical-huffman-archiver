#include "archiver.h"

#include "filereader.h"
#include "filewriter.h"
#include "huffman.h"
#include "trie.h"

#include <cstddef>
#include <vector>

Archiver::Archiver(const std::string& archive_path, const std::vector<std::string>& file_paths)
    : archive_path_(archive_path), file_paths_(file_paths)
{
}

Archiver::Archiver(const std::string& archive_path) : archive_path_(archive_path) { }

void Archiver::Compress()
{
    FileWriter writer(archive_path_);

    for (size_t i = 0; i < file_paths_.size(); ++i)
    {
        FileReader reader(file_paths_[i]);
        bool is_last_file = i == file_paths_.size() - 1;

        HuffmanCoder huffman_coder(reader, writer, is_last_file);
        huffman_coder.Encode();
    }
}

void Archiver::Decompress()
{
    FileReader reader(archive_path_);

    bool need_to_decode_next_file = true;
    while (need_to_decode_next_file)
    {
        uint16_t symbols_count = reader.ReadHuffmanInt();
        Symbols symbols = RestoreSymbols(symbols_count, reader);
        auto symbols_counts_with_same_code_lengths
            = RestoreSymbolsCountsWithSameCodeLengths(symbols, reader);
        BinaryTrie trie(symbols, symbols_counts_with_same_code_lengths);

        std::string file_name = RestoreFileName(reader, trie);
        FileWriter writer(file_name);

        while (true)
        {
            uint16_t symbol = trie.GetCharacter(reader);
            if (symbol == FILENAME_END)
            {
                break;
            }
            writer.WriteCharacter(static_cast<char>(symbol));
        }

        need_to_decode_next_file = trie.GetCharacter(reader) == ONE_MORE_FILE;
    }
}

Symbols Archiver::RestoreSymbols(uint16_t symbols_count, FileReader& reader)
{
    Symbols symbols(symbols_count);
    for (size_t i = 0; i < symbols_count; ++i)
    {
        uint16_t symbol = reader.ReadHuffmanInt();
        symbols[i] = symbol;
    }
    return symbols;
}

std::vector<size_t> Archiver::RestoreSymbolsCountsWithSameCodeLengths(Symbols symbols,
                                                                      FileReader& reader)
{
    std::vector<size_t> symbols_counts_with_same_code_lengths;

    size_t symbols_read = 0;
    while (symbols_read < symbols.size())
    {
        uint16_t count_of_symbols_with_specific_code_length = reader.ReadHuffmanInt();
        symbols_counts_with_same_code_lengths.push_back(count_of_symbols_with_specific_code_length);
        symbols_read += count_of_symbols_with_specific_code_length;
    }

    return symbols_counts_with_same_code_lengths;
}

std::string Archiver::RestoreFileName(FileReader& reader, BinaryTrie& trie)
{
    std::string file_name;
    while (!file_name.ends_with(".txt"))
    {
        file_name += trie.GetCharacter(reader);
    }
    return file_name;
}
