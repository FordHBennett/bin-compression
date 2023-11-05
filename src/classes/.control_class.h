#pragma once

#include <cstddef>
#include <iostream>
#include <vector>
#include <cassert> // Add this line to include the assert function
#include "../functions/file_functions.h"

class Control_Stats {
public:
    // Constructors
    Control_Stats();
    Control_Stats(double avgSizeBytes, double average_time_encoded_in_ns , double average_time_encoded_in_ns ,
            double average_compression_ratio, size_t avgPeakMemoryDuringEncoding,
            size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput);



    std::vector<char> encode(const std::vector<char>& input);
    std::vector<char> decode(const std::vector<char>& input);

    //functions
    void Print_Stats();
    void Calculate_Cumulative_Average_Stats_For_Directory(int divisors);
    void getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* decodedFilename, size_t fileSize, std::filesystem::path& currentDir);
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