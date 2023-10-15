#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <queue>
#include <map>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <string>

void generateRandomBinFile(const char* filename, long long fileSize, double zeroProbability) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
        return;
    }

    // Determine the number of zero bytes to write
    long long zeroBytes = static_cast<long long>(zeroProbability * fileSize);

    for (long long i = 0; i < zeroBytes; ++i) {
        char byte = 0;  // Write zero bytes
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    // Determine the number of non-zero bytes to write
    long long nonZeroBytes = fileSize - zeroBytes;

    std::default_random_engine generator;
    std::bernoulli_distribution distribution(0.5); // 50% probability for non-zero bits

    for (long long i = 0; i < nonZeroBytes; ++i) {
        bool bit = distribution(generator);
        char byte = static_cast<char>(bit);
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    outFile.close();
}


double getFileSize(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
        return 1;
    }

    // Seek to the end to get the file size
    file.seekg(0, std::ios::end);
    double fileSize = file.tellg();

    // Close the file
    file.close();

    if (fileSize < 0) {
        std::cerr << "Error: Unable to determine the file size." << std::endl;
        return 1;
    }
    return fileSize;
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


// Function to perform LZW encoding
std::vector<int> lzwEncode(const std::vector<char>& input) {
    std::map<std::string, int> dictionary;
    std::vector<int> lzwEncoded;

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        std::string next = current + c;
        if (dictionary.find(next) != dictionary.end()) {
            // If the current + c is in the dictionary, update the current string
            current = next;
        } else {
            // Output the code for the current string and add next to the dictionary
            lzwEncoded.push_back(dictionary[current]);
            dictionary[next] = dictionary.size();
            current = std::string(1, c);
        }
    }

    if (!current.empty()) {
        // Output the code for the remaining current string
        lzwEncoded.push_back(dictionary[current]);
    }

    return lzwEncoded;
}

// Function to perform LZW decoding
std::vector<char> lzwDecode(const std::vector<int>& input) {
    std::map<int, std::string> dictionary;
    std::vector<char> lzwDecoded;

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary[input[0]];
    lzwDecoded.insert(lzwDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.find(code) != dictionary.end()) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZW decoding error: Invalid code.");
        }

        lzwDecoded.insert(lzwDecoded.end(), entry.begin(), entry.end());

        // Add a new entry to the dictionary
        dictionary[dictionary.size()] = current + entry[0];

        current = entry;
    }

    return lzwDecoded;
}

// Function to perform bitwise compression
std::vector<bool> bitwiseCompress(const std::vector<char>& input) {
    std::vector<bool> compressedBits;
    if (input.empty()) {
        return compressedBits;
    }

    bool current = input[0];
    int count = 1;

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == current) {
            count++;
        } else {
            // Append the bit and its count to the compressed data
            compressedBits.push_back(current);
            compressedBits.push_back(count % 2); // Using count % 2 to represent even/odd count
            current = input[i];
            count = 1;
        }
    }

    // Append the last bit and its count
    compressedBits.push_back(current);
    compressedBits.push_back(count % 2);

    return compressedBits;
}

// Function to perform bitwise decompression
std::vector<char> bitwiseDecompress(const std::vector<bool>& input) {
    std::vector<char> decompressedData;

    for (size_t i = 0; i < input.size(); i += 2) {
        bool bit = input[i];
        int count = input[i + 1] ? 1 : 2; // Convert even/odd count back to actual count
        for (int j = 0; j < count; ++j) {
            decompressedData.push_back(static_cast<char>(bit));
        }
    }

    return decompressedData;
}
int main() {
    const int numIterations = 10;
    const char* filename = "random_binary_file.bin";
    const char* runLengthFilename = "run_length_encoded.bin";
    const char* runLengthDecodedFilename = "run_length_decoded.bin";
    const char* lzwEncodedFilename = "lzw_encoded.bin";
    const char* lzwDecodedFilename = "lzw_decoded.bin";
    const char* bitwiseEncodedFilename = "bitwise_compressed.bin";
    const char* bitwiseDecodedFilename = "bitwise_decompressed.bin";
    long long file_size = 10000000; // Adjust the file size as needed
    double zeroProbability = 0.5; // Adjust the probability of 0s

    double avgRunLengthSizeBytes = 0;
    double avgRunLengthTimeMs = 0;
    double avgLzwSizeBytes = 0;
    double avgLzwTimeMs = 0;
    double avgBitwiseSizeBytes = 0;
    double avgBitwiseTimeMs = 0;

    for (int i = 0; i < numIterations; ++i) {

        generateRandomBinFile(filename, file_size, zeroProbability);

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

        std::vector<char> runLengthDecoded = runLengthDecode(runLengthEncoded);
        bool dataMatches = binaryData == runLengthDecoded;
        std::cout << "Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

        // Write the run-length encoded and decoded files
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

        // open the run-length encoded file and determine the file size
        avgRunLengthSizeBytes += getFileSize(runLengthFilename);
        avgRunLengthTimeMs += durationRunLength.count();

        // Perform LZW encoding
        auto startLzw = std::chrono::high_resolution_clock::now();
        std::vector<int> lzwEncoded = lzwEncode(binaryData);
        auto stopLzw = std::chrono::high_resolution_clock::now();
        auto durationLzw = std::chrono::duration_cast<std::chrono::milliseconds>(stopLzw - startLzw);

        // Perform LZW decoding
        std::vector<char> lzwDecoded = lzwDecode(lzwEncoded);

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzwDataMatches = binaryData == lzwDecoded;
        std::cout << "LZW Data Matches: " << (lzwDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZW encoded data for further verification
        std::ofstream lzwOutFile(lzwEncodedFilename, std::ios::binary);
        std::ofstream lzwDecodedOutFile(lzwDecodedFilename, std::ios::binary);
        if (!lzwOutFile || !lzwDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZW encoded or decoded file." << std::endl;
            return 1;
        }

        for (size_t i = 0; i < lzwEncoded.size(); ++i) {
            int code = lzwEncoded[i];
            lzwOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
        }

        for (size_t i = 0; i < lzwDecoded.size(); ++i) {
            char byte = lzwDecoded[i];
            lzwDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        lzwOutFile.close();
        lzwDecodedOutFile.close();

        // open the LZW encoded file and determine the file size
        avgLzwSizeBytes += getFileSize(lzwEncodedFilename);
        avgLzwTimeMs += durationLzw.count();

        // Perform bitwise compression
        auto startBitwise = std::chrono::high_resolution_clock::now();
        std::vector<bool> bitwiseCompressed = bitwiseCompress(binaryData);
        auto stopBitwise = std::chrono::high_resolution_clock::now();
        auto durationBitwise = std::chrono::duration_cast<std::chrono::milliseconds>(stopBitwise - startBitwise);

        // Perform bitwise decompression
        std::vector<char> bitwiseDecompressed = bitwiseDecompress(bitwiseCompressed);

        // Verify that no data is lost by comparing decoded data with the original data
        bool bitwiseDataMatches = binaryData == bitwiseDecompressed;
        std::cout << "Bitwise Data Matches: " << (bitwiseDataMatches ? "Yes" : "No") << std::endl;

        // Calculate the size in bytes for the bitwise compressed data
        avgBitwiseSizeBytes += (bitwiseCompressed.size() / 8); // Convert bits to bytes

        avgBitwiseTimeMs += durationBitwise.count();
    }

    avgRunLengthSizeBytes /= numIterations;
    avgRunLengthTimeMs /= numIterations;
    avgLzwSizeBytes /= numIterations;
    avgLzwTimeMs /= numIterations;
    avgBitwiseSizeBytes /= numIterations;
    avgBitwiseTimeMs /= numIterations;

    std::cout << "Average Run-Length Encoded File Size: " << avgRunLengthSizeBytes << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Time: " << avgRunLengthTimeMs << " ms" << std::endl;
    std::cout << "Average LZW Encoded File Size: " << avgLzwSizeBytes << " bytes" << std::endl;
    std::cout << "Average LZW Encoding Time: " << avgLzwTimeMs << " ms" << std::endl;
    std::cout << "Average Bitwise Compressed File Size: " << avgBitwiseSizeBytes << " bytes" << std::endl;
    std::cout << "Average Bitwise Compression Time: " << avgBitwiseTimeMs << " ms" << std::endl;


    return 0;
}