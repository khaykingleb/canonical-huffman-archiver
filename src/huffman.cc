#include "huffman.h"

#include "filereader.h"
#include "filewriter.h"
#include "trie.h"

#include <_types/_uint16_t.h>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

HuffmanCoder::HuffmanCoder(FileReader& reader, FileWriter& writer, bool is_last_file)
    : reader_(reader), writer_(writer), is_last_file_(is_last_file)
{
}

void HuffmanCoder::Encode() const
{
    // Build the Huffman codes
    HuffmanCodes codes = BuildCodes();
    auto [canonical_codes, canonical_codes_for_lookup] = MoveCodesToCanonicalForm(codes);

    // Write the number of characters in the file
    writer_.WriteHuffmanInt(canonical_codes.size());

    // Write the characters
    for (const auto& [key, _] : canonical_codes)
    {
        auto character = key.second;
        writer_.WriteHuffmanInt(character);
    }

    // Write a list of sizes of each character group in the canonical codes
    size_t previous_code_length = 0;
    size_t count_of_codes_with_same_length = 0;

    /// Write zeros from up to the first code length
    auto first_code_length = canonical_codes.begin()->first.first;
    for (size_t missing_length = 1; missing_length < first_code_length; ++missing_length)
    {
        writer_.WriteHuffmanInt(0);
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
                writer_.WriteHuffmanInt(count_of_codes_with_same_length);

                // Write zero for any missing code lengths between the previous and current
                for (size_t missing_length = previous_code_length + 1;
                     missing_length < current_code_length;
                     ++missing_length)
                {
                    writer_.WriteHuffmanInt(0);
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
        writer_.WriteHuffmanInt(count_of_codes_with_same_length);
    }

    // Reset the reader position to the start of the file
    reader_.ResetPositionToStart();

    // Write the file name
    std::string file_name = reader_.GetFileName();
    for (const auto& character : file_name)
    {
        auto it = canonical_codes_for_lookup.find(character);
        if (it != canonical_codes_for_lookup.end())
        {
            std::string canonical_code = it->second;
            writer_.WriteHuffmanCode(canonical_code);
        }
    }

    // Write the file content
    while (reader_.HasMoreCharacters())
    {
        auto character = reader_.ReadCharacter();
        if (character.has_value())
        {
            auto it = canonical_codes_for_lookup.find(*character);
            if (it != canonical_codes_for_lookup.end())
            {
                std::string canonical_code = it->second;
                writer_.WriteHuffmanCode(canonical_code);
            }
        }
    }

    // Write the end of the file
    std::string end_of_file_code = canonical_codes_for_lookup.at(FILENAME_END);
    writer_.WriteHuffmanCode(end_of_file_code);

    if (!is_last_file_)
    {
        // Write the start of the next file in the archive
        std::string one_more_file_code = canonical_codes_for_lookup.at(ONE_MORE_FILE);
        writer_.WriteHuffmanCode(one_more_file_code);
    }
    else
    {
        // Write the end of the archive
        std::string archive_end_code = canonical_codes_for_lookup.at(ARCHIVE_END);
        writer_.WriteHuffmanCode(archive_end_code);
    }
}

HuffmanCodes HuffmanCoder::BuildCodes() const
{
    auto character_frequencies = GetCharacterFrequencies();
    BinaryTrie trie(character_frequencies);
    auto root = trie.GetRoot();

    HuffmanCodes codes;
    GenerateHuffmanCodes(root, "", codes);

    return codes;
}

CharacterFrequencies HuffmanCoder::GetCharacterFrequencies() const
{
    CharacterFrequencies frequency_table;

    // Count the frequency of each character in the file name
    auto file_name = reader_.GetFileName();
    for (const auto& character : file_name)
    {
        ++frequency_table[static_cast<unsigned char>(character)];
    }

    // Count the frequency of each character in the file content
    while (reader_.HasMoreCharacters())
    {
        auto character = reader_.ReadCharacter();
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

std::tuple<HuffmanCodes, HuffmanCodesForLookup> MoveCodesToCanonicalForm(HuffmanCodes codes)
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
