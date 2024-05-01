#include "filereader.h"
#include "filewriter.h"

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

/*
 * Special codes for the Huffman coding.
 */
constexpr uint16_t FILENAME_END = 256;
constexpr uint16_t ONE_MORE_FILE = 257;
constexpr uint16_t ARCHIVE_END = 258;

/*
 * Data structures to build the Huffman binary trie.
 */
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

using MinHeap
    = std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodeCompare>;

/*
 * Data structures to create the Huffman codes.
 */
using CharacterFrequencies = std::unordered_map<uint16_t, uint64_t>;
using HuffmanKey = std::pair<size_t, uint16_t>; // (code length, character)

struct HuffmanKeyCompare
{
    bool operator()(const HuffmanKey& lhs, const HuffmanKey& rhs) const
    {
        if (lhs.first == rhs.first)
        {
            return lhs.second < rhs.second;
        }
        return lhs.first < rhs.first;
    }
};

using HuffmanCodes = std::map<HuffmanKey, std::string, HuffmanKeyCompare>;
using HuffmanCodesForLookup = std::unordered_map<uint16_t, std::string>;

/*
 * Class to encode the file using Huffman coding algorithm.
 */
class HuffmanCoder
{
public:
    /*
     * Constructor.
     * @param reader The file reader.
     * @param writer The file writer.
     */
    HuffmanCoder(FileReader& reader, FileWriter& writer);

    /*
     * Encode the file using Huffman coding algorithm.
     */
    void Encode() const;

protected:
    /*
     * Build the canonical Huffman codes.
     */
    HuffmanCodes BuildCodes() const;

    /*
     * Get the character frequencies from the file.
     * @return The character frequencies.
     */
    CharacterFrequencies GetCharacterFrequencies() const;

    /*
     * Build the Huffman binary trie from the character frequencies using a priority queue
     * with bottom-up approach.
     * @param character_frequencies The character frequencies.
     * @return The root of the Huffman binary trie.
     */
    std::shared_ptr<Node> GetTrie(const CharacterFrequencies& character_frequencies) const;

    /*
     * Generate the Huffman codes.
     * @param node The current node in the Huffman binary trie
     * @param code The current code
     */
    void GenerateHuffmanCodes(const std::shared_ptr<Node>& node,
                              const std::string& code,
                              HuffmanCodes& codes) const;

private:
    FileReader& reader_;
    FileWriter& writer_;
};

/*
 * Move the Huffman codes to canonical form.
 */
std::tuple<HuffmanCodes, HuffmanCodesForLookup> MoveCodesToCanonicalForm(HuffmanCodes codes);
