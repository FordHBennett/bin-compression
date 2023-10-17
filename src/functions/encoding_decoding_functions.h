#pragma once

#ifndef ENCODING_DECODING_FUNCTIONS_H
#define ENCODING_DECODING_FUNCTIONS_H

#include <vector>
#include <unordered_map>

// Function prototypes for Run-length encoding/decoding
std::vector<std::pair<char, int>> runLengthEncode(const std::vector<char>& input);
std::vector<char> runLengthDecode(const std::vector<std::pair<char, int>>& input);

// Function prototypes for LZW encoding/decoding
std::vector<int> lzwEncode(const std::vector<char>& input);
std::vector<char> lzwDecode(const std::vector<int>& input);

// Function prototypes for LZP encoding/decoding
std::vector<int> lzpEncode(const std::vector<char>& input);
std::vector<char> lzpDecode(const std::vector<int>& input);

// Function prototypes for Burrows-Wheeler Transform (BWT) encoding/decoding
std::vector<char> burrowsWheelerEncode(const std::vector<char>& input);
std::vector<char> burrowsWheelerDecode(const std::vector<char>& input);

// Function to get peak memory usage
size_t getPeakMemoryUsage();

#endif // ENCODING_DECODING_FUNCTIONS_H
