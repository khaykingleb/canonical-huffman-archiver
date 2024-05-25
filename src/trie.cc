#include "trie.h"

#include <bitset>

BinaryTrie::BinaryTrie(const CharacterFrequencies& character_frequencies)
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

    root_ = min_heap.top();
}

BinaryTrie::BinaryTrie(const Symbols& symbols,
                       const std::vector<size_t>& symbols_counts_with_same_code_lengths)
{
    std::vector<std::string> huffman_codes;

    uint64_t current_huffman_code = 0;
    for (size_t code_length = 1; code_length <= symbols_counts_with_same_code_lengths.size();
         ++code_length)
    {
        for (size_t i = 0; i < symbols_counts_with_same_code_lengths[code_length - 1]; ++i)
        {
            std::string huffman_code
                = std::bitset<64>(current_huffman_code).to_string().substr(64 - code_length);
            huffman_codes.push_back(huffman_code);

            ++current_huffman_code;
        }

        current_huffman_code <<= 1;
    }

    root_ = std::make_shared<Node>();
    for (size_t i = 0; i < symbols.size(); ++i)
    {
        auto node = root_;
        for (size_t j = 0; j < huffman_codes[i].size(); ++j)
        {
            if (huffman_codes[i][j] == '0')
            {
                if (!node->left)
                {
                    node->left = std::make_shared<Node>();
                }
                node = node->left;
            }
            else
            {
                if (!node->right)
                {
                    node->right = std::make_shared<Node>();
                }
                node = node->right;
            }
        }
        node->character = symbols[i];
    }
}

std::shared_ptr<Node> BinaryTrie::GetRoot() const
{
    return root_;
}

uint16_t BinaryTrie::GetCharacter(FileReader& reader) const
{
    auto node = root_;
    while (node->left || node->right)
    {
        if (reader.ReadBit())
        {
            node = node->right;
        }
        else
        {
            node = node->left;
        }
    }
    return node->character;
}
