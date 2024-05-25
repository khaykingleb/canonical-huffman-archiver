#pragma once

#include "filereader.h"
#include "filewriter.h"
#include "trie.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/*
 * Special control codes for the Huffman coding.
 */
constexpr uint16_t FILENAME_END = 256;
constexpr uint16_t ONE_MORE_FILE = 257;
constexpr uint16_t ARCHIVE_END = 258;

/*
 * Type for the Huffman codes.
 */
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
     * Move the generated codes to the canonical form.
     * @param codes The generated codes
     * @return The canonical Huffman codes and the codes for fast lookup
     */
    std::tuple<HuffmanCodes, HuffmanCodesForLookup>
    MoveCodesToCanonicalForm(HuffmanCodes codes) const;

    /*
     * Encode the file using Huffman coding algorithm.
     * @param reader The file reader.
     * @param writer The file writer.
     * @param is_last_file Is this the last file in the archive.
     */
    void Encode(FileReader& reader, FileWriter& writer, bool is_last_file = false) const;

    /*
     * Decode the file using Huffman coding algorithm.
     * @param reader The file reader.
     * @return True if there are more files to decode, false otherwise.
     */
    bool Decode(FileReader& reader) const;

protected:
    /*
     * Build the canonical Huffman codes.
     * @param reader The file reader.
     */
    HuffmanCodes BuildCodes(FileReader& reader) const;

    /*
     * Get the character frequencies from the file.
     * @param reader The file reader.
     * @return The character frequencies.
     */
    CharacterFrequencies GetCharacterFrequencies(FileReader& reader) const;

    /*
     * Generate the Huffman codes using recursive traversal of the Huffman binary trie.
     * @param node The current node in the Huffman binary trie
     * @param code The current code
     * @param codes The generated codes
     */
    void GenerateHuffmanCodes(const std::shared_ptr<Node>& node,
                              const std::string& code,
                              HuffmanCodes& codes) const;

    /*
     * Get the symbols from the encoded file.
     * @param symbols_count The number of symbols to read.
     * @param reader The file reader to read the symbols from the encoded file.
     * @return The symbols read from the file.
     */
    Symbols RestoreSymbols(uint16_t symbols_count, FileReader& reader) const;

    /*
     * Get the symbol counts with the same code lengths from the encoded file.
     * @param symbols The symbols to get the codes for.
     * @param reader The file reader to read the counts from the encoded file.
     * @return The counts of symbols with the same code lengths.
     */
    std::vector<size_t> RestoreSymbolsCountsWithSameCodeLengths(Symbols symbols,
                                                                FileReader& reader) const;

    /*
     * Restore the file name from the encoded file.
     * @param reader The file reader to read the file name from the encoded file.
     * @param trie The trie to use for decoding the file name.
     * @return The file name.
     */
    std::string RestoreFileName(FileReader& reader, BinaryTrie& trie) const;

    /*
     * Restore the content of the file from the encoded file and write it to the output file.
     * @param reader The file reader to read the content from the encoded file.
     * @param writer The file writer to write the content to the output file.
     * @param trie The trie to use for decoding the content.
     */
    void RestoreAndWriteContent(FileReader& reader, FileWriter& writer, BinaryTrie& trie) const;
};
