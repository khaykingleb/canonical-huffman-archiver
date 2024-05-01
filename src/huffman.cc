#include "huffman.h"

#include "filereader.h"
#include "filewriter.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

HuffmanCoder::HuffmanCoder(FileReader& reader, FileWriter& writer)
    : reader_(reader), writer_(writer)
{
}

HuffmanCodes HuffmanCoder::BuildCodes()
{
    auto character_frequencies = GetCharacterFrequencies();
    auto root = BuildTrie(character_frequencies);

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
        frequency_table[character]++;
    }

    // Count the frequency of each character in the file content
    while (reader_.HasMoreCharacters())
    {
        auto character = reader_.ReadCharacter();
        if (character.has_value())
        {
            frequency_table[*character]++;
        }
    }

    // Add special codes to the frequency table
    std::vector<uint16_t> special_codes = { ARCHIVE_END, FILENAME_END, ONE_MORE_FILE };
    for (const auto& special_code : special_codes)
    {
        frequency_table[special_code]++;
    }

    return frequency_table;
}

std::shared_ptr<Node>
HuffmanCoder::BuildTrie(const CharacterFrequencies& character_frequencies) const
{
    // Build a min heap of nodes
    MinHeap min_heap;
    for (const auto& [character, frequency] : character_frequencies)
    {
        min_heap.emplace(
            std::make_shared<Node>(Node { .character = character, .frequency = frequency }));
    }

    // Build the trie by merging nodes with the smallest frequencies
    while (min_heap.size() > 1)
    {
        auto left = min_heap.top();
        min_heap.pop();

        auto right = min_heap.top();
        min_heap.pop();

        min_heap.emplace(std::make_shared<Node>(Node {
            .frequency = left->frequency + right->frequency, .left = left, .right = right }));
    }

    // Return the root node
    return min_heap.top();
}

void HuffmanCoder::GenerateHuffmanCodes(const std::shared_ptr<Node>& node,
                                        const std::string& code,
                                        HuffmanCodes& codes)
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

        current_code++;
        previous_code_bit_length = code_bit_length;
    }

    return { canonical_codes, canonical_codes_for_lookup };
}

void HuffmanCoder::Encode()
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

    // Write the canonical Huffman codes
    for (const auto& [_, canonical_code] : canonical_codes)
    {
        size_t num_bits_for_writing = canonical_code.size();
        writer_.WriteHuffmanInt(std::stoll(canonical_code, nullptr, 2), num_bits_for_writing);
    };

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
            size_t num_bits_for_writing = canonical_code.size();
            writer_.WriteHuffmanInt(std::stoll(canonical_code, nullptr, 2), num_bits_for_writing);
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
                size_t num_bits_for_writing = canonical_code.size();
                writer_.WriteHuffmanInt(std::stoll(canonical_code, nullptr, 2),
                                        num_bits_for_writing);
            }
        }
    }
}
