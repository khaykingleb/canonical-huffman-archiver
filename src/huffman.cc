#include "huffman.h"
#include "filereader.h"
#include "filewriter.h"
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>


HuffmanCoder::HuffmanCoder(FileWriter& writer, FileReader& reader)
    : writer_(writer)
    , reader_(reader)
{
}


void HuffmanCoder::BuildCanonicalCodes()
{
    auto character_frequencies = GetCharacterFrequencies();
    auto binary_trie = BuildBinaryTrie(character_frequencies);
    GenerateHuffmanCodes(binary_trie, "");
    MoveHuffmanCodesToCanonicalForm();
}

HuffmanCodes HuffmanCoder::GetCodes() const
{
    return codes_;
}

std::unordered_map<uint16_t, uint64_t> HuffmanCoder::GetCharacterFrequencies() const
{
    std::unordered_map<uint16_t, uint64_t> frequency_table;

    // Count the frequency of each character in the file name
    std::string file_name = reader_.GetFileName();
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

std::shared_ptr<Node> HuffmanCoder::BuildBinaryTrie(
    const std::unordered_map<uint16_t, uint64_t>& character_frequencies) const
{
    // Build a min heap of nodes
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodeCompare>
        min_heap;
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


void HuffmanCoder::GenerateHuffmanCodes(const std::shared_ptr<Node>& node, const std::string& code)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->left == nullptr && node->right == nullptr)
    {
        codes_.insert({ node->character, code });
        return;
    }

    if (node->left != nullptr)
    {
        GenerateHuffmanCodes(node->left, code + "0");
    }

    if (node->right != nullptr)
    {
        GenerateHuffmanCodes(node->right, code + "1");
    }
}

void HuffmanCoder::MoveHuffmanCodesToCanonicalForm()
{
    std::map<size_t, std::vector<uint16_t>> code_bit_lengths;
    for (const auto& [character, code] : codes_)
    {
        code_bit_lengths[code.size()].emplace_back(character);
    }

    size_t current_code_bit_length = 0;
    uint16_t current_code = 0;
    for (const auto& [code_bit_length, characters] : code_bit_lengths)
    {
        current_code <<= (code_bit_length - current_code_bit_length);
        for (const auto& character : characters)
        {
            auto canonical_code = std::bitset<16>(current_code).to_string();
            auto trimmed_canonical_code = canonical_code.substr(16 - code_bit_length);
            codes_.insert({ character, trimmed_canonical_code });

            current_code++;
        }

        current_code_bit_length = code_bit_length;
    }
}


void HuffmanCoder::Encode()
{
    // TODO: SHOULD BE REALLY SORTED
    // for (const auto& [character, code] : codes_)
    // {
    //     std::cout << character << " " << code << std::endl;
    // }

    // Write the number of characters in the file
    writer_.WriteBits(codes_.size(), 9);

    // Write the alphabet characters
    for (const auto& [character, _] : codes_)
    {
        writer_.WriteBits(character, 9);
    }

    // auto symbols_count = reader_.ReadHuffmanInt(9);
    // std::cout << "symbols_count: " << symbols_count << std::endl;

    // for (size_t i = 0; i < symbols_count; ++i)
    // {
    //     auto character = reader_.ReadHuffmanInt(9);
    //     std::cout << "character: " << character << std::endl;
    // }

    // Write the canonical Huffman codes
    // for (const auto& [_, code] : codes_)
    // {
    //     for (const auto& bit : code)
    //     {
    //         writer_.WriteBits(bit - '0', 1);
    //     }
    // }


    // writer_.WriteBits(codes_.size(), 9);
}
