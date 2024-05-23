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

std::shared_ptr<Node> BinaryTrie::GetRoot() const
{
    return root_;
}
