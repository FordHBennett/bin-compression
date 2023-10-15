#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <queue>
#include <map>
#include <algorithm>
#include <bitset>

void generateRandomBinFile(const char* filename, long long fileSize, double zeroProbability) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
        return;
    }
    std::default_random_engine generator;
    std::bernoulli_distribution distribution(zeroProbability);

    for (long long i = 0; i < fileSize; ++i) {
        bool bit = distribution(generator);
        char byte = static_cast<char>(bit);
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    outFile.close();
}

// Function to perform run-length encoding
std::vector<std::pair<bool, int>> runLengthEncode(const std::vector<char>& input) {
    std::vector<std::pair<bool, int>> runLengthEncoded;
    if (input.empty()) {
        return runLengthEncoded;
    }

    bool current = input[0];
    int count = 1;

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == current) {
            count++;
        } else {
            runLengthEncoded.push_back(std::make_pair(current, count));
            current = input[i];
            count = 1;
        }
    }

    // After the loop, don't forget to add the last run.
    runLengthEncoded.push_back(std::make_pair(current, count));

    return runLengthEncoded;
}

// Function to perform run-length decoding
std::vector<char> runLengthDecode(const std::vector<std::pair<bool, int>>& input) {
    std::vector<char> runLengthDecoded;

    for (const auto& pair : input) {
        for (int i = 0; i < pair.second; i++) {
            runLengthDecoded.push_back(static_cast<char>(pair.first));
        }
    }

    return runLengthDecoded;
}

int main() {
    const char* filename = "random_binary_file.bin";
    const char* runLengthFilename = "run_length_encoded.bin";
    const char* runLengthDecodedFilename = "run_length_decoded.bin";
    long long file_size = 10000000; // Adjust the file size as needed
    double zeroProbability = 0.5; // Adjust the probability of 0s

    generateRandomBinFile(filename, file_size, zeroProbability);
    std::cout << "Random binary file generated: " << filename << std::endl;

    // Read the binary file
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
        return 1;
    }

    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<char> binaryData(fileSize);
    inFile.read(binaryData.data(), fileSize);
    inFile.close();

    // Perform run-length encoding
    auto startRunLength = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<bool, int>> runLengthEncoded = runLengthEncode(binaryData);
    auto stopRunLength = std::chrono::high_resolution_clock::now();
    auto durationRunLength = std::chrono::duration_cast<std::chrono::milliseconds>(stopRunLength - startRunLength);

    // Perform run-length decoding
    std::vector<char> runLengthDecoded = runLengthDecode(runLengthEncoded);

    // Calculate sizes in bits and bytes
    size_t runLengthSizeBits = runLengthEncoded.size();
    size_t runLengthDecodedSizeBits = runLengthDecoded.size();

    size_t runLengthSizeBytes = runLengthSizeBits / 8; // Divide by 8 to get bytes
    size_t runLengthDecodedSizeBytes = runLengthDecodedSizeBits / 8; // Divide by 8 to get bytes

    // Calculate percent differences
    double runLengthPercentDiff = (static_cast<double>(runLengthSizeBits) - static_cast<double>(fileSize * 8)) / static_cast<double>(fileSize * 8) * 100.0;
    double runLengthDecodedPercentDiff = (static_cast<double>(runLengthDecodedSizeBits) - static_cast<double>(fileSize * 8)) / static_cast<double>(fileSize * 8) * 100.0;

    // Output statistics
    std::cout << "Original File Size: " << fileSize << " bytes" << std::endl;
    std::cout << "Run-Length Encoded File Size: " << runLengthSizeBytes << " bytes" << std::endl;
    std::cout << "Run-Length Decoded File Size: " << runLengthDecodedSizeBytes << " bytes" << std::endl;
    std::cout << "Run-Length Encoding Time: " << durationRunLength.count() << " ms" << std::endl;
    std::cout << "Percent Difference (Run-Length Encoding): " << runLengthPercentDiff << "%" << std::endl;
    std::cout << "Percent Difference (Run-Length Decoding): " << runLengthDecodedPercentDiff << "%" << std::endl;

    // Verify that no data is lost by comparing decoded data with the original data
    bool dataMatches = binaryData == runLengthDecoded;
    std::cout << "Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

    // Create a binary file from run-length encoded data for further verification
    std::ofstream runLengthOutFile(runLengthFilename, std::ios::binary);
    std::ofstream runLengthDecodedOutFile(runLengthDecodedFilename, std::ios::binary);
    if (!runLengthOutFile || !runLengthDecodedOutFile) {
        std::cerr << "Error: Unable to create the run-length encoded or decoded file." << std::endl;
        return 1;
    }

    for (size_t i = 0; i < runLengthEncoded.size(); ++i) {
        char byte = static_cast<char>(runLengthEncoded[i].first);
        runLengthOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    for (size_t i = 0; i < runLengthDecoded.size(); ++i) {
        char byte = runLengthDecoded[i];
        runLengthDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    runLengthOutFile.close();
    runLengthDecodedOutFile.close();
    return 0;
}