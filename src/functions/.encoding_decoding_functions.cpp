#include "encoding_decoding_functions.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <mach/mach.h>
#include <mach/task.h>


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
        std::cerr << "Error: Unable to retrieve task information." << "\n";
        return SIZE_MAX; // indicate an error
    }

    return t_info.resident_size;
}
