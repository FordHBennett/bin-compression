#include "encoding_decoding_functions.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <mach/mach.h>
#include <mach/task.h>


// Run-length encoding
// Time Complexity:
// Best, Average, Worst: O(n)

// Space Complexity:
// Best, Average, Worst: O(n)
std::vector<std::pair<char, int>> runLengthEncode(const std::vector<char>& input) {
    if (input.empty()) {
        return {};
    }

    std::vector<std::pair<char, int>> runLengthEncoded;
    runLengthEncoded.reserve(input.size()); // Reserve the maximum potential size.

    char currentChar = input[0];
    int runLength = 1;

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == currentChar && runLength < 255) {
            runLength++;
        } else {
            runLengthEncoded.emplace_back(currentChar, runLength);
            currentChar = input[i];
            runLength = 1;
        }
    }

    // After the loop, don't forget to add the last run.
    runLengthEncoded.emplace_back(currentChar, runLength);

    runLengthEncoded.shrink_to_fit(); // Reduce capacity to fit the actual size.

    return runLengthEncoded;
}

// Run-length decoding
// Time Complexity:
// Best, Average, Worst: O(n)

// Space Complexity:
// Best, Average, Worst: O(n)

std::vector<char> runLengthDecode(const std::vector<std::pair<char, int>>& input) {
    std::vector<char> runLengthDecoded;
    size_t totalLength = 0;

    // Calculate the total length to reserve capacity.
    for (const auto& pair : input) {
        totalLength += pair.second;
    }
    runLengthDecoded.reserve(totalLength);

    for (const auto& pair : input) {
        std::fill_n(std::back_inserter(runLengthDecoded), pair.second, pair.first);
    }

    return runLengthDecoded;
}

// Function to perform LZW encoding
// Time Complexity:
// Best: O(n) for completely repetitive data.
// Average:O(nlogn) as building the dictionary may require up to O(logn) time for lookups.
// Worst: O(nlogn) due to lookups in the dictionary.

// Space Complexity:
// Best: O(n) for completely repetitive data.
// Average:O(n) for the dictionary plus the output.
// Worst:O(n) for the dictionary plus the output.
std::vector<int> lzwEncode(const std::vector<char>& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzwEncoded;
    lzwEncoded.reserve(input.size());  // Reduce potential reallocations

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            current.pop_back();  // Remove last character
            lzwEncoded.push_back(dictionary[current]);
            dictionary[current + c] = dictionary.size();
            current = c;
        }
    }

    if (!current.empty()) {
        lzwEncoded.push_back(dictionary[current]);
    }

    return lzwEncoded;
}

// Function to perform LZW decoding
// Time Complexity:
// Best, Average, Worst:O(n)Processing each code in the input requires constant
// time since the dictionary is built as the decoding progresses.
// Space Complexity:
// Best, Average, Worst:O(n) The dictionary's size grows with the input, plus the output size.


std::vector<char> lzwDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzwDecoded;
    lzwDecoded.reserve(input.size());  // Reduce potential reallocations

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzwDecoded.insert(lzwDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZW decoding error: Invalid code.");
        }

        lzwDecoded.insert(lzwDecoded.end(), entry.begin(), entry.end());

        // Add a new entry to the dictionary
        dictionary[dictionary.size()] = current + entry[0];

        current = entry;
    }

    return lzwDecoded;
}


// Lempel-Ziv Parallel (LZP) encoding
// Time Complexity:
// Best, Average:O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst: O(n)

std::vector<int> lzpEncode(const std::vector<char>& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzpEncoded;
    lzpEncoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            current.pop_back();  // Remove the last character
            lzpEncoded.push_back(dictionary[current]);
            dictionary[current + c] = dictionary.size();
            current = c;
        }
    }

    if (!current.empty()) {
        lzpEncoded.push_back(dictionary[current]);
    }

    return lzpEncoded;
}

// Lempel-Ziv Parallel (LZP) decoding
// Time Complexity:
// Best, Average:O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst: O(n)
std::vector<char> lzpDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzpDecoded;
    lzpDecoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzpDecoded.insert(lzpDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZP decoding error: Invalid code.");
        }

        lzpDecoded.insert(lzpDecoded.end(), entry.begin(), entry.end());

        dictionary[dictionary.size()] = current + entry[0];
        current = entry;
    }

    return lzpDecoded;
}

// Time Complexity:
// Best, Average: O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst:O(n)


std::vector<char> burrowsWheelerEncode(const std::vector<char>& input) {
    size_t len = input.size();

    // BWT
    std::vector<const char*> rotations(len);
    for (size_t i = 0; i < len; ++i) {
        rotations[i] = &input[i];
    }
    std::sort(rotations.begin(), rotations.end(), [&input, len](const char* a, const char* b) {
        return std::lexicographical_compare(a, a + len, b, b + len);
    });
    std::vector<char> bwtResult(len);
    for (size_t i = 0; i < len; ++i) {
        bwtResult[i] = *(rotations[i] == &input[0] ? &input.back() : rotations[i] - 1);
    }

    // MTF
    std::vector<char> alphabet(256);
    std::iota(alphabet.begin(), alphabet.end(), 0);
    std::vector<char> mtfResult(len);
    for (size_t i = 0; i < len; ++i) {
        char c = bwtResult[i];
        int position = std::distance(alphabet.begin(), std::find(alphabet.begin(), alphabet.end(), c));
        mtfResult[i] = static_cast<char>(position);
        std::rotate(alphabet.begin(), alphabet.begin() + position, alphabet.begin() + position + 1);
    }
    return mtfResult;
}

// Time Complexity:
// Best, Average, Worst: O(n) All operations are linear with respect to the input size.
// Space Complexity:
//Best, Average, Worst: O(n)
std::vector<char> burrowsWheelerDecode(const std::vector<char>& input) {
    size_t len = input.size();

    // Inverse MTF
    std::vector<char> alphabet(256);
    std::iota(alphabet.begin(), alphabet.end(), 0);
    std::vector<char> invMtfResult(len);
    for (size_t i = 0; i < len; ++i) {
        char pos = input[i];
        char c = alphabet[pos];
        invMtfResult[i] = c;
        std::rotate(alphabet.begin(), alphabet.begin() + pos, alphabet.begin() + pos + 1);
    }

    // Inverse BWT
    std::vector<int> count(256, 0);
    for (char c : invMtfResult) {
        count[static_cast<unsigned char>(c)]++;
    }
    std::vector<int> next(len, 0);
    std::vector<int> position(256, 0);
    for (int i = 1; i < 256; ++i) {
        position[i] = position[i - 1] + count[i - 1];
    }
    for (size_t i = 0; i < len; ++i) {
        next[position[static_cast<unsigned char>(invMtfResult[i])]++] = i;
    }
    std::vector<char> invBwtResult(len);
    int idx = 0;
    for (size_t i = 0; i < len; ++i) {
        invBwtResult[i] = invMtfResult[idx];
        idx = next[idx];
    }
    return invBwtResult;
}




size_t getPeakMemoryUsage() {
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count)) {
        std::cerr << "Error: Unable to retrieve task information." << std::endl;
        return SIZE_MAX; // indicate an error
    }

    return t_info.resident_size;
}
