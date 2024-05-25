#include "huffman.h"

#include <gtest/gtest.h>

TEST(HuffmanCoderTest, MoveCodesToCanonicalForm)
{
    HuffmanCodes correct_canonical_codes {
        { { 1, 1 }, "0" }, { { 2, 0 }, "10" }, { { 3, 2 }, "110" }, { { 3, 3 }, "111" }
    };
    HuffmanCodes codes {
        { { 1, 1 }, "0" }, { { 2, 0 }, "11" }, { { 3, 2 }, "101" }, { { 3, 3 }, "100" }
    };

    HuffmanCoder huffman_coder;
    auto [canonical_codes, _] = huffman_coder.MoveCodesToCanonicalForm(codes);
    for (const auto& [key, code] : canonical_codes)
    {
        ASSERT_EQ(code, correct_canonical_codes[key]);
    }
}
