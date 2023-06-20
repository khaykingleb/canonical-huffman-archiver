# Canonical Huffman Coding

Program that compresses and decompresses ASCII files based on [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding#:~:text=In%20computer%20science%20and%20information,used%20for%20lossless%20data%20compression.&text=The%20output%20from%20Huffman's%20algorithm,a%20character%20in%20a%20file) in a [canonical](https://en.wikipedia.org/wiki/Canonical_Huffman_code) manner.

## Getting Started

```shell

```

## Commands

* `/archiver -h` displays help for using the program.
* `./archiver -c archive_name file1 [file2 ...]` encodes the files `fil1, file2, ...` and saves the result to the file `archive_name`.
* `./archiver -d archive_name` decodes the files from the archive `archive_name` and puts them in the current directory.

## Algorithm

The compression algorithm is structured as follows:

1. The frequency of 8-bit characters in the file is calculated. In addition to the contents of the file, it is necessary to take into account the frequency of characters in the file name, and also add three service characters `FILENAME_END = 256`,`ONE_MORE_FILE = 257`, `ARCHIVE_END = 258` with frequencies of 1. The purpose of these characters will be described later. Thus, 9 bits are needed to encode the extended alphabet.
2. A binary [trie](https://en.wikipedia.org/wiki/Trie) encoding is constructed by the following procedure:

   1. For each character of the alphabet, a corresponding vertex is added to the priority queue. The ordering of the vertices in the queue is carried out according to the non-decreasing frequency of characters in the file (at the "head" of the queue there is always the vertex with the character with the least occurrence in the file).
   2. While there is more than one element in the queue, two vertices A and B with minimum priorities are sequentially retrieved from it. A new vertex C is created, the children of which are vertices A and B. Vertex C is placed in a queue with a priority equal to the sum of the priorities of vertices A and B.
   3. At the end of the procedure, exactly one vertex remains in the queue, which is the root of the constructed trie. Leaf tops are terminal. Each terminal vertex contains a  haracter from the source file. Each nonterminal vertex of the tree contains two edges: left and right, which are marked with bits 0 and 1, respectively. Each terminal vertex corresponds to a bit sequence obtained by descending from the root of the boron to the terminal vertex and writing out the bits of all traversed edges.

   * [gif demo](https://commons.wikimedia.org/wiki/File:Huffmantree_ru_animated.gif?uselang=ru)
   * [gif demo](https://commons.wikimedia.org/wiki/File:Huffman_huff_demo.gif)
   * [graphic demo](https://people.ok.ubc.ca/ylucet/DS/Huffman.html).
3. All symbols are assigned a binary code sequence by means of the constructed boron.
4. The code is converted to [canonical form](https://en.wikipedia.org/wiki/Canonical_Huffman_code). The canonical form of the code differs in that it allows you to unambiguously recover codes from the list of symbols and the lengths of the codes for them.
5. All symbols of the file are replaced with the corresponding code binary sequences, and the result is written together with auxiliary information into the file. The archive file format is described below.

The decoding algorithm is structured as follows:

1. The coding table (correspondence between sivols and their codes) is restored from the file.
2. A binary trie is constructed according to the coding table.
3. Binary sequences read from the input file are traced along the trie from the root to the leaf vertices. When the next leaf vertex of the trie is reached, the corresponding symbol is determined, which is written to the output file.

## File format

Nine-bit values are written in low-to-high order format (analogous to little-endian for bits). Those. first comes the bit corresponding to `2^0`, then `2^1`, etc. to the bit corresponding to `2^8`. For a program to be portable, you cannot rely on the order of the bits in the file to match the order of the bits in the binary representation of the numbers in memory.

The archive file has the following format:

1. 9 bits numbers of characters in the alphabet `SYMBOLS_COUNT`.
2. Data block for recovering the canonical code.

   1. `SYMBOLS_COUNT` values of 9 bits - alphabet characters in the order of canonical codes.
   2. A list of `MAX_SYMBOL_CODE_SIZE` values of 9 bits, the i-th (when numbered from 0) element of which is the number of characters with the code length `i + 1`. `MAX_SYMBOL_CODE_SIZE` - maximum code length in the current encoding. The canonical codes correspond in order to the characters from the previous paragraph. `MAX_SYMBOL_CODE_SIZE` is not written explicitly to the file, because it can be restored from the available data.
3. The encoded file name.
4. The encoded service symbol `FILENAME_END`.
5. The encoded content of the file.
6. If there are more files in the archive, then the encoded service symbol `ONE_MORE_FILE` and the encoding continues.
7. The encoded service symbol `ARCHIVE_END`.
