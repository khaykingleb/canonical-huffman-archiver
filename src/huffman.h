#include "filereader.h"
#include "filewriter.h"
#include "trie.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

/*
 * Special control codes for the Huffman coding.
 */
constexpr uint16_t FILENAME_END = 256;
constexpr uint16_t ONE_MORE_FILE = 257;
constexpr uint16_t ARCHIVE_END = 258;

using HuffmanKey = std::pair<size_t, uint16_t>; // (code length, character)

struct HuffmanKeyCompare
{
    bool operator()(const HuffmanKey& lhs, const HuffmanKey& rhs) const
    {
        return std::tie(lhs.first, lhs.second) < std::tie(rhs.first, rhs.second);
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
    HuffmanCoder(FileReader& reader, FileWriter& writer, bool is_last_file = false);

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
    bool is_last_file_;
};

/*
 * Move the Huffman codes to canonical form.
 */
std::tuple<HuffmanCodes, HuffmanCodesForLookup> MoveCodesToCanonicalForm(HuffmanCodes codes);
