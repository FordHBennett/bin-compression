#pragma once

#include <cstddef>
#include <iostream>
#include <cassert> // Add this line to include the assert function

#include "../functions/file_functions.h"

class LZP_Stats {
public:
    // Constructors
    LZP_Stats();
    LZP_Stats(double avgSizeBytes, double average_time_encoded_in_ns , double average_time_encoded_in_ns ,
            double average_compression_ratio, size_t avgPeakMemoryDuringEncoding,
            size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput);

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
    void Print_Stats();
    void Calculate_Cumulative_Average_Stats_For_Directory(int divisors);
    void getFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize, std::filesystem::path& currentDir);
    void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir);

    // Setters
    void setAvgSizeBytes(double value);
    void setaverage_time_encoded_in_ns (double value);
    void setaverage_time_encoded_in_ns (double value);
    void setaverage_compression_ratio(double value);
    void setAvgPeakMemoryDuringEncoding(size_t value);
    void setAvgPeakMemoryDuringDecoding(size_t value);
    void setaverage_encoded_throughput(double value);
    void setaverage_decoded_throughput(double value);

    // Getters
    double getAvgSizeBytes() const;
    double getaverage_time_encoded_in_ns () const;
    double getaverage_time_encoded_in_ns () const;
    double getaverage_compression_ratio() const;
    size_t getAvgPeakMemoryDuringEncoding() const;
    size_t getAvgPeakMemoryDuringDecoding() const;
    double getaverage_encoded_throughput() const;
    double getaverage_decoded_throughput() const;

private:
    double avgSizeBytes;
    double average_time_encoded_in_ns ;
    double average_time_encoded_in_ns ;
    double average_compression_ratio;
    size_t avgPeakMemoryDuringEncoding;
    size_t avgPeakMemoryDuringDecoding;
    double average_encoded_throughput;
    double average_decoded_throughput;
};