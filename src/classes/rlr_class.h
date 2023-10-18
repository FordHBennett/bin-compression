#pragma once

#include <cstddef>
#include <iostream>
#include "../functions/file_functions.h"

class RLR_Stats {
public:
    // Constructors
    RLR_Stats();
    RLR_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs,
            double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding,
            size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded);


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

    //functions
    void printStats();
    void calculateAvgStats(int divisors);
    void getFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize);

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