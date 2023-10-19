#pragma once

#include <cstddef>
#include <iostream>
#include "../functions/file_functions.h"
#include <unordered_map>
#include <queue>
#include <vector>
#include <chrono>
#include <fstream>

class Huffman_Stats {
public:
    // Constructors
    Huffman_Stats();
    ~Huffman_Stats();

    // Encoding and decoding functions
    std::vector<char> encode(const std::vector<char>& input);
    std::vector<char> decode(const std::vector<char>& input);

    // Helper functions
    void printStats();
    void calculateAvgStats(int divisor);
    void getFileStats(std::vector<char>& binaryData, const char* huffmanEncodedFileName, const char* huffmanDecodedFileName, size_t fileSize);
    void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations);

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
    struct Node {
        char character;
        int frequency;
        Node* left;
        Node* right;

        Node(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
    };

    Node* buildHuffmanTree(const std::unordered_map<char, int>& frequencies);
    void generateHuffmanCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& codes);
    void deleteTree(Node* root);

    Node* huffmanTreeRoot;
    std::unordered_map<char, std::string> huffmanCodes;
    std::unordered_map<std::string, char> reverseHuffmanCodes;

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
