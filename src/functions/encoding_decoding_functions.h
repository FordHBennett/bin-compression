#pragma once

#ifndef ENCODING_DECODING_FUNCTIONS_H
#define ENCODING_DECODING_FUNCTIONS_H

#include <vector>
#include <unordered_map>

/**
 * The function `runLengthEncode` takes a vector of characters as input and returns a vector of pairs,
 * where each pair represents a character and its run length in the input vector.
 *
 * @param input A vector of characters that represents the input sequence.
 *
 * @return The function `runLengthEncode` returns a `std::vector<std::pair<char, int>>`.
 */
std::vector<std::pair<char, int>> runLengthEncode(const std::vector<char>& input);

/**
 * The function `runLengthDecode` takes a vector of pairs representing characters and their
 * corresponding run lengths, and returns a vector of characters where each character is repeated
 * according to its run length.
 *
 * @param input A vector of pairs, where each pair consists of a character and an integer. The
 * character represents a repeated sequence of characters, and the integer represents the number of
 * times the character is repeated.
 *
 * @return The function `runLengthDecode` returns a `std::vector<char>` containing the decoded sequence
 * of characters.
 */
std::vector<char> runLengthDecode(const std::vector<std::pair<char, int>>& input);

/**
 * The function `lzwEncode` takes a vector of characters as input and returns a vector of integers
 * representing the LZW encoding of the input.
 *
 * @param input The input parameter is a vector of characters (std::vector<char>) that represents the
 * input data to be encoded using the LZW algorithm.
 *
 * @return The function `lzwEncode` returns a `std::vector<int>` containing the LZW encoded
 * representation of the input.
 */
std::vector<int> lzwEncode(const std::vector<char>& input);

/**
 * The `lzwDecode` function takes a vector of integers as input and decodes it using the LZW algorithm,
 * returning a vector of characters.
 *
 * @param input The input parameter is a vector of integers representing the LZW-encoded data.
 *
 * @return The function `lzwDecode` returns a `std::vector<char>` containing the decoded output of the
 * LZW algorithm.
 */
std::vector<char> lzwDecode(const std::vector<int>& input);

/**
 * The `lzpEncode` function takes a vector of characters as input and returns a vector of integers
 * representing the Lempel-Ziv-Welch encoding of the input.
 *
 * @param input The input parameter is a vector of characters, which represents the input data that
 * needs to be encoded using the Lempel-Ziv-Welch (LZW) algorithm.
 *
 * @return The function `lzpEncode` returns a `std::vector<int>` which contains the Lempel-Ziv-Welch
 * (LZW) encoded representation of the input vector of characters.
 */
std::vector<int> lzpEncode(const std::vector<char>& input);

/**
 * The function `lzpDecode` decodes a given input vector of integers using the LZP (Lempel-Ziv-Parse)
 * algorithm and returns the decoded vector of characters.
 *
 * @param input The input parameter is a vector of integers, which represents the encoded data using
 * LZP (Lempel-Ziv-Parseval) compression algorithm.
 *
 * @return The function `lzpDecode` returns a `std::vector<char>`, which contains the decoded output of
 * the LZP (Lempel-Ziv-Parse) algorithm.
 */
std::vector<char> lzpDecode(const std::vector<int>& input);

/**
 * The function `burrowsWheelerEncode` takes an input vector of characters, performs Burrows-Wheeler
 * Transform (BWT) and Move-to-Front (MTF) encoding on it, and returns the encoded result as a vector
 * of characters.
 *
 * @param input The input parameter is a vector of characters, which represents the input string that
 * we want to encode using the Burrows-Wheeler Transform (BWT) and Move-to-Front (MTF) algorithms.
 *
 * @return The function `burrowsWheelerEncode` returns a `std::vector<char>` which contains the result
 * of the Burrows-Wheeler Transform (BWT) and Move-to-Front (MTF) encoding applied to the input vector.
 */
std::vector<char> burrowsWheelerEncode(const std::vector<char>& input);

/**
 * The function `burrowsWheelerDecode` decodes a Burrows-Wheeler transformed input string.
 *
 * @param input The input parameter is a vector of characters, which represents the encoded
 * Burrows-Wheeler Transform (BWT) result.
 *
 * @return The function `burrowsWheelerDecode` returns a `std::vector<char>` which contains the decoded
 * result after performing the inverse Burrows-Wheeler Transform (BWT) and inverse Move-to-Front (MTF)
 * operations on the input vector.
 */
std::vector<char> burrowsWheelerDecode(const std::vector<char>& input);

/**
 * The function `getPeakMemoryUsage` retrieves the peak memory usage of the current process in C++.
 *
 * @return the peak memory usage of the current task in bytes.
 */
size_t getPeakMemoryUsage();

#endif // ENCODING_DECODING_FUNCTIONS_H
