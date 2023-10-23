#pragma once

#include <cstddef>
#include <iostream>
#include <cassert> // Add this line to include the assert function

#include "../functions/file_functions.h"

class LZP_Stats {
public:
    // Constructors
    LZP_Stats();
    LZP_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs,
            double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding,
            size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded);

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

    // Functions
    void printStats();
    void calculateAvgStats(int divisors);
    void getFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize, std::filesystem::path& currentDir);
    void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir);

    // Setters
    void setAvgSizeBytes(double value);
    void setAvgEncodedTimeMs(double value);
    void setAvgDecodedTimeMs(double value);
    void setAvgCompressionRatio(double value);
    void setAvgPeakMemoryDuringEncoding(size_t value);
    void setAvgPeakMemoryDuringDecoding(size_t value);
    void setAvgEncodedThroughput(double value);
    void setAvgThroughputDecoded(double value);

    // Getters
    double getAvgSizeBytes() const;
    double getAvgEncodedTimeMs() const;
    double getAvgDecodedTimeMs() const;
    double getAvgCompressionRatio() const;
    size_t getAvgPeakMemoryDuringEncoding() const;
    size_t getAvgPeakMemoryDuringDecoding() const;
    double getAvgEncodedThroughput() const;
    double getAvgThroughputDecoded() const;

private:
    double avgSizeBytes;
    double avgEncodedTimeMs;
    double avgDecodedTimeMs;
    double avgCompressionRatio;
    size_t avgPeakMemoryDuringEncoding;
    size_t avgPeakMemoryDuringDecoding;
    double avgEncodedThroughput;
    double avgThroughputDecoded;
};