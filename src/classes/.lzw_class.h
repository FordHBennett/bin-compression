#pragma once

#include <cstddef>
#include <iostream>
#include <cassert> // Add this line to include the assert function
#include "../functions/file_functions.h"

class LZW_Stats {
public:
    // Constructors
    LZW_Stats();
    LZW_Stats(double avgSizeBytes, double average_time_encoded_in_microseconds , double average_time_encoded_in_microseconds , double average_compression_ratio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput);

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
    void Print_Stats();
    void Calculate_Cumulative_Average_Stats_For_Directory(int divisors);
    void getFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize, std::filesystem::path& currentDir);
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
    double avgSizeBytes;
    double average_time_encoded_in_microseconds ;
    double average_time_encoded_in_microseconds ;
    double average_compression_ratio;
    size_t avgPeakMemoryDuringEncoding;
    size_t avgPeakMemoryDuringDecoding;
    double average_encoded_throughput;
    double average_decoded_throughput;
};