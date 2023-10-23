#pragma once

#include <cstddef>
#include <iostream>
#include <cassert> // Add this line to include the assert function
#include "../functions/file_functions.h"

class LZW_Stats {
public:
    // Constructors
    LZW_Stats();
    LZW_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded);

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