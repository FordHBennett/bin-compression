#pragma once

#include <cstddef>
#include <iostream>
#include "../functions/file_functions.h"
#include "huffman.h"
#include "node.h"
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <vector>
#include <chrono>
#include <fstream>

class Huffman_Stats {
public:
    // Constructors
    Huffman_Stats();



    // Helper functions
    void printStats();
    void calculateAvgStats(int divisor);
    void getFileStats(const char* filename, const char* huffmanEncodedFileName, const char* huffmanDecodedFileName, size_t fileSize, std::filesystem::path& currentDir);
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
    // Helper functions

    // Stats attributes
    double avgSizeBytes;
    double avgEncodedTimeMs;
    double avgDecodedTimeMs;
    double avgCompressionRatio;
    size_t avgPeakMemoryDuringEncoding;
    size_t avgPeakMemoryDuringDecoding;
    double avgEncodedThroughput;
    double avgThroughputDecoded;
};
