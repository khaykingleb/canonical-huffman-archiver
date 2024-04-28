#include "filereader.h"
#include "filewriter.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

/*
 * Special codes for the Huffman coding.
 */
const uint16_t FILENAME_END = 256;
const uint16_t ONE_MORE_FILE = 257;
const uint16_t ARCHIVE_END = 258;

/**
 * Node of the Huffman tree.
 */
struct Node
{
    uint16_t character;
    uint64_t frequency;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
};

// Comparator for priority queue
struct NodeCompare
{
    bool operator()(const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs)
    {
        return lhs->frequency > rhs->frequency;
    }
};

// Comparator for sorting the codes
struct CodeCompare
{
    bool operator()(const std::pair<uint16_t, std::string>& lhs,
                    const std::pair<uint16_t, std::string>& rhs) const
    {
        // Compare lexicographically if the lengths are equal
        if (lhs.second.size() == rhs.second.size())
        {
            return lhs.first < rhs.first;
        }
        // Otherwise, compare by the length of the code
        return lhs.second.size() < rhs.second.size();
    }
};

using HuffmanCodes = std::set<std::pair<uint16_t, std::string>, CodeCompare>;

class HuffmanCoder
{
public:
    // Constructor
    HuffmanCoder(FileWriter& writer, FileReader& reader);

    void BuildCanonicalCodes();
    HuffmanCodes GetCodes() const;
    void Encode();

protected:
    std::unordered_map<uint16_t, uint64_t> GetCharacterFrequencies() const;
    std::shared_ptr<Node>
    BuildBinaryTrie(const std::unordered_map<uint16_t, uint64_t>& character_frequencies) const;
    void GenerateHuffmanCodes(const std::shared_ptr<Node>& node, const std::string& code);
    void MoveHuffmanCodesToCanonicalForm();

private:
    FileWriter& writer_;
    FileReader& reader_;

    // std::unordered_map<uint16_t, std::string> codes_;
    HuffmanCodes codes_;
};
