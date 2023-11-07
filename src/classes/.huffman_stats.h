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
    void Print_Stats();
    void Calculate_Cumulative_Average_Stats_For_Directory(int divisor);
    void getFileStats(const char* filename, const char* huffmanEncodedFileName, const char* huffmanDecodedFileName, size_t fileSize, std::filesystem::path& currentDir);
    void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir);

    // Setters
    void setAvgSizeBytes(double value);
    void setaverage_time_encoded_in_microseconds (double value);
    void setaverage_time_encoded_in_microseconds (double value);
    void setaverage_compression_ratio(double value);
    void setAvgPeakMemoryDuringEncoding(size_t value);
    void setAvgPeakMemoryDuringDecoding(size_t value);
    void setaverage_encoded_throughput(double value);
    void setaverage_decoded_throughput(double value);

    // Getters
    double getAvgSizeBytes() const;
    double getaverage_time_encoded_in_microseconds () const;
    double getaverage_time_encoded_in_microseconds () const;
    double getaverage_compression_ratio() const;
    size_t getAvgPeakMemoryDuringEncoding() const;
    size_t getAvgPeakMemoryDuringDecoding() const;
    double getaverage_encoded_throughput() const;
    double getaverage_decoded_throughput() const;

private:
    // Helper functions

    // Stats attributes
    double avgSizeBytes;
    double average_time_encoded_in_microseconds ;
    double average_time_encoded_in_microseconds ;
    double average_compression_ratio;
    size_t avgPeakMemoryDuringEncoding;
    size_t avgPeakMemoryDuringDecoding;
    double average_encoded_throughput;
    double average_decoded_throughput;
};
