#pragma once

#ifndef ENCODING_DECODING_FUNCTIONS_H
#define ENCODING_DECODING_FUNCTIONS_H

#include <vector>
#include <unordered_map>


std::vector<char> lzpDecode(const std::vector<int>& input);

/**
 * The function `burrowsWheelerEncode` takes an input vector of characters, performs Burrows-Wheeler
 * Transformation (BWT) and Move-to-Front (MTF) encoding on it, and returns the encoded result as a vector
 * of characters.
 *
 * @param input The input parameter is a vector of characters, which represents the input string that
 * we want to encode using the Burrows-Wheeler Transformation (BWT) and Move-to-Front (MTF) algorithms.
 *
 * @return The function `burrowsWheelerEncode` returns a `std::vector<char>` which contains the result
 * of the Burrows-Wheeler Transformation (BWT) and Move-to-Front (MTF) encoding applied to the input vector.
 */
std::vector<char> burrowsWheelerEncode(const std::vector<char>& input);

/**
 * The function `burrowsWheelerDecode` decodes a Burrows-Wheeler Transformationed input string.
 *
 * @param input The input parameter is a vector of characters, which represents the encoded
 * Burrows-Wheeler Transformation (BWT) result.
 *
 * @return The function `burrowsWheelerDecode` returns a `std::vector<char>` which contains the decoded
 * result after performing the inverse Burrows-Wheeler Transformation (BWT) and inverse Move-to-Front (MTF)
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
