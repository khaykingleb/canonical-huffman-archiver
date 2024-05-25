#pragma once

#include "filereader.h"

#include <cstddef>
#include <memory>
#include <queue>
#include <unordered_map>

struct Node
{
    uint16_t character;
    uint64_t frequency;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
};

struct NodeCompare
{
    bool operator()(const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs)
    {
        return lhs->frequency > rhs->frequency;
    }
};

using Symbols = std::vector<uint16_t>;
using CharacterFrequencies = std::unordered_map<uint16_t, uint64_t>;
using MinHeap
    = std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodeCompare>;

class BinaryTrie
{
public:
    /*
     * Constructor.
     * Build the trie from the character frequencies using a priority queue with bottom-up approach.
     * @param character_frequencies The character frequencies.
     */
    BinaryTrie(const CharacterFrequencies& character_frequencies);

    /*
     * Constructor.
     * Build the trie from the symbols and the Huffman codes.
     * @param symbols The symbols.
     * @param symbols_counts_with_same_code_lengths The counts of symbols with the same code
     * lengths.
     */
    BinaryTrie(const Symbols& symbols,
               const std::vector<size_t>& symbols_counts_with_same_code_lengths);

    /*
     * Get the root of the trie.
     * @return The root of the trie.
     */
    std::shared_ptr<Node> GetRoot() const;

    /*
     * Get the character from the trie.
     * @param reader The file reader to read the character from the trie.
     * @return The character.
     */
    uint16_t GetCharacter(FileReader& reader) const;

private:
    std::shared_ptr<Node> root_;
};
