# Canonical Huffman Coding

Program that compresses and decompresses ASCII files based on [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding#:~:text=In%20computer%20science%20and%20information,used%20for%20lossless%20data%20compression.&text=The%20output%20from%20Huffman's%20algorithm,a%20character%20in%20a%20file) in a [canonical](https://en.wikipedia.org/wiki/Canonical_Huffman_code) manner.

## Getting Started

* In general, use the following to run the archiver program:
   ```shell
   make build && make test && make run
   ```

* For local development, you can attempt to use:
   ```shell
   make local-init && make conan-build
   ```

## Commands

* `./archiver -h` displays help for using the program.
* `./archiver -c archive_name file1 [file2 ...]` encodes the files `fil1, file2, ...` and saves the result to the file `archive_name`.
* `./archiver -d archive_name` decodes the files from the archive `archive_name` and puts them in the current directory.

## File format

Nine-bit values are written in low-to-high order format (analogous to little-endian for bits). That is, the bit corresponding to `2^0` comes first, followed by `2^1`, and so on, up to the bit corresponding to `2^9`.

The archive file has the following format:

1. A 9-bit number indicating the number of characters in the alphabet `SYMBOLS_COUNT`.
2. Data block for recovering the canonical code:

   1. `SYMBOLS_COUNT` values of 9 bits (alphabet characters in the order of canonical codes).
   2. A list of `MAX_SYMBOL_CODE_SIZE` values of 9 bits, the `i`-th (when numbered from 0) element of which is the number of characters with the code length `i + 1`. `MAX_SYMBOL_CODE_SIZE` , the maximum code length in the current encoding, is not explicitly written to the file because it can be deduced from the available data.
3. The encoded file name.
4. The encoded content of the file.
5. The encoded service symbol `FILENAME_END`.
6. If there are additional files in the archive, the encoded service symbol `ONE_MORE_FILE` is used, and the encoding continues.
7. The encoded service symbol `ARCHIVE_END`.
