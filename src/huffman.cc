#include "huffman.h"

#include "filereader.h"
#include "filewriter.h"
#include "trie.h"

std::tuple<HuffmanCodes, HuffmanCodesForLookup>
HuffmanCoder::MoveCodesToCanonicalForm(HuffmanCodes codes) const
{
    HuffmanCodes canonical_codes;
    HuffmanCodesForLookup canonical_codes_for_lookup;

    size_t previous_code_bit_length = 0;
    uint16_t current_code = 0;
    for (const auto& [key, code] : codes)
    {
        auto [code_bit_length, character] = key;

        current_code <<= (code_bit_length - previous_code_bit_length);
        auto canonical_code
            = std::bitset<16>(current_code).to_string().substr(16 - code_bit_length);

        canonical_codes.emplace(std::make_pair(code_bit_length, character), canonical_code);
        canonical_codes_for_lookup.emplace(character, canonical_code);

        ++current_code;
        previous_code_bit_length = code_bit_length;
    }

    return { canonical_codes, canonical_codes_for_lookup };
}

void HuffmanCoder::Encode(FileReader& reader, FileWriter& writer, bool is_last_file) const
{
    HuffmanCodes codes = BuildCodes(reader);
    auto [canonical_codes, canonical_codes_for_lookup] = MoveCodesToCanonicalForm(codes);

    // Write the number of characters in the file
    writer.WriteHuffmanInt(canonical_codes.size());

    // Write the characters
    for (const auto& [key, _] : canonical_codes)
    {
        auto character = key.second;
        writer.WriteHuffmanInt(character);
    }

    // Write a list of sizes of each character group in the canonical codes
    size_t previous_code_length = 0;
    size_t count_of_codes_with_same_length = 0;

    /// Write zeros from up to the first code length
    auto first_code_length = canonical_codes.begin()->first.first;
    for (size_t missing_length = 1; missing_length < first_code_length; ++missing_length)
    {
        writer.WriteHuffmanInt(0);
    }

    /// Write the count of codes with the same length
    for (const auto& [key, _] : canonical_codes)
    {
        size_t current_code_length = key.first;
        if (current_code_length != previous_code_length)
        {
            if (previous_code_length != 0)
            {
                // Write the count for the previous code length
                writer.WriteHuffmanInt(count_of_codes_with_same_length);

                // Write zero for any missing code lengths between the previous and current
                for (size_t missing_length = previous_code_length + 1;
                     missing_length < current_code_length;
                     ++missing_length)
                {
                    writer.WriteHuffmanInt(0);
                }
            }

            previous_code_length = current_code_length;
            count_of_codes_with_same_length = 0;
        }

        ++count_of_codes_with_same_length;
    }

    /// Final write for the last group of codes
    if (count_of_codes_with_same_length != 0)
    {
        writer.WriteHuffmanInt(count_of_codes_with_same_length);
    }

    // Reset the reader position to the start of the file
    reader.ResetPositionToStart();

    // Write the file name
    std::string file_name = reader.GetFileName();
    for (const auto& character : file_name)
    {
        auto it = canonical_codes_for_lookup.find(character);
        if (it != canonical_codes_for_lookup.end())
        {
            std::string canonical_code = it->second;
            writer.WriteHuffmanCode(canonical_code);
        }
    }

    // Write the file content
    while (reader.HasMoreCharacters())
    {
        auto character = reader.ReadCharacter();
        if (character.has_value())
        {
            auto it = canonical_codes_for_lookup.find(*character);
            if (it != canonical_codes_for_lookup.end())
            {
                std::string canonical_code = it->second;
                writer.WriteHuffmanCode(canonical_code);
            }
        }
    }

    // Write the end of the file
    std::string end_of_file_code = canonical_codes_for_lookup.at(FILENAME_END);
    writer.WriteHuffmanCode(end_of_file_code);

    if (!is_last_file)
    {
        // Write the start of the next file in the archive
        std::string one_more_file_code = canonical_codes_for_lookup.at(ONE_MORE_FILE);
        writer.WriteHuffmanCode(one_more_file_code);
    }
    else
    {
        // Write the end of the archive
        std::string archive_end_code = canonical_codes_for_lookup.at(ARCHIVE_END);
        writer.WriteHuffmanCode(archive_end_code);
    }
}

bool HuffmanCoder::Decode(FileReader& reader) const
{
    auto symbols_count = reader.ReadHuffmanInt();
    auto symbols = RestoreSymbols(symbols_count, reader);
    auto symbols_counts_with_same_code_lengths
        = RestoreSymbolsCountsWithSameCodeLengths(symbols, reader);
    BinaryTrie trie(symbols, symbols_counts_with_same_code_lengths);

    std::string file_name = RestoreFileName(reader, trie);
    FileWriter writer(file_name);
    RestoreAndWriteContent(reader, writer, trie);

    bool need_to_decode_next_file = trie.GetCharacter(reader) == ONE_MORE_FILE;
    return need_to_decode_next_file;
}

HuffmanCodes HuffmanCoder::BuildCodes(FileReader& reader) const
{
    auto character_frequencies = GetCharacterFrequencies(reader);
    BinaryTrie trie(character_frequencies);

    HuffmanCodes codes;
    auto root = trie.GetRoot();
    GenerateHuffmanCodes(root, "", codes);

    return codes;
}

CharacterFrequencies HuffmanCoder::GetCharacterFrequencies(FileReader& reader) const
{
    CharacterFrequencies frequency_table;

    // Count the frequency of each character in the file name
    auto file_name = reader.GetFileName();
    for (const auto& character : file_name)
    {
        ++frequency_table[static_cast<unsigned char>(character)];
    }

    // Count the frequency of each character in the file content
    while (reader.HasMoreCharacters())
    {
        auto character = reader.ReadCharacter();
        if (character.has_value())
        {
            ++frequency_table[*character];
        }
    }

    // Add special codes to the frequency table
    std::vector<uint16_t> special_codes = { ARCHIVE_END, FILENAME_END, ONE_MORE_FILE };
    for (const auto& special_code : special_codes)
    {
        ++frequency_table[special_code];
    }

    return frequency_table;
}

void HuffmanCoder::GenerateHuffmanCodes(const std::shared_ptr<Node>& node,
                                        const std::string& code,
                                        HuffmanCodes& codes) const
{
    if (node == nullptr)
    {
        return;
    }

    if (node->left == nullptr && node->right == nullptr)
    {
        codes.emplace(std::make_pair(code.size(), node->character), code);
        return;
    }

    if (node->left != nullptr)
    {
        GenerateHuffmanCodes(node->left, code + "0", codes);
    }

    if (node->right != nullptr)
    {
        GenerateHuffmanCodes(node->right, code + "1", codes);
    }
}

Symbols HuffmanCoder::RestoreSymbols(uint16_t symbols_count, FileReader& reader) const
{
    Symbols symbols(symbols_count);
    for (size_t i = 0; i < symbols_count; ++i)
    {
        uint16_t symbol = reader.ReadHuffmanInt();
        symbols[i] = symbol;
    }
    return symbols;
}

std::vector<size_t> HuffmanCoder::RestoreSymbolsCountsWithSameCodeLengths(Symbols symbols,
                                                                          FileReader& reader) const
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

std::string HuffmanCoder::RestoreFileName(FileReader& reader, BinaryTrie& trie) const
{
    std::string file_name;
    while (!file_name.ends_with(".txt"))
    {
        file_name += trie.GetCharacter(reader);
    }
    return file_name;
}

void HuffmanCoder::RestoreAndWriteContent(FileReader& reader,
                                          FileWriter& writer,
                                          BinaryTrie& trie) const
{
    while (auto symbol = trie.GetCharacter(reader))
    {
        if (symbol == FILENAME_END)
        {
            break;
        }
        writer.WriteCharacter(static_cast<char>(symbol));
    }
}
