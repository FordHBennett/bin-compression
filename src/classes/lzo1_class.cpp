#include "lzo1_class.h"

// Constructors
LZO1_Stats::LZO1_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

LZO1_Stats::LZO1_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

std::vector<char> LZO1_Stats::encode(const std::vector<char>& input) {
    std::vector<char> output;
    std::unordered_map<std::string, int> dictionary;
    int dictSize = 256;
    std::string currentStr;
    std::string prevStr;

    for (char c : input) {
        currentStr += c;
        if (dictionary.find(currentStr) == dictionary.end()) {
            if (prevStr.empty()) {
                output.push_back(c);
            } else {
                int code = dictionary[prevStr];
                output.push_back(static_cast<char>(code));
                output.push_back(c);
                dictionary[currentStr] = dictSize++;
            }
            prevStr.clear();
            currentStr.clear();
        } else {
            prevStr = currentStr;
        }
    }

    if (!prevStr.empty()) {
        int code = dictionary[prevStr];
        output.push_back(static_cast<char>(code));
    }

    return output;
}

std::vector<char> LZO1_Stats::decode(const std::vector<char>& input) {
    std::vector<char> output;
    std::unordered_map<int, std::string> dictionary;
    int dictSize = 256;

    for (int i = 0; i < 256; i++) {
        dictionary[i] = std::string(1, i);
    }

    for (size_t i = 0; i < input.size();) {
        char c = input[i];
        if (i + 1 < input.size() && dictionary.find(static_cast<int>(c)) != dictionary.end()) {
            output.push_back(dictionary[static_cast<int>(c)][0]);
            output.push_back(input[i + 1]);
            dictionary[dictSize++] = dictionary[static_cast<int>(c)] + input[i + 1];
            i += 2;
        } else {
            output.push_back(c);
            i++;
        }
    }

    return output;
}


// Functions
void LZO1_Stats::printStats() {
    std::cout << "LZO1: Average size (bytes): " << avgSizeBytes << std::endl;
    std::cout << "LZO1: Average encoded time (ms): " << avgEncodedTimeMs << std::endl;
    std::cout << "LZO1: Average decoded time (ms): " << avgDecodedTimeMs << std::endl;
    std::cout << "LZO1: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "LZO1: Average peak memory during encoding (bytes): " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "LZO1: Average peak memory during decoding (bytes): " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "LZO1: Average encoded throughput (bytes/s): " << avgEncodedThroughput << std::endl;
    std::cout << "LZO1: Average decoded throughput (bytes/s): " << avgThroughputDecoded << std::endl;
}

void LZO1_Stats::calculateAvgStats(int divisor) {
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void LZO1_Stats::getFileStats(std::vector<char>& binaryData, const char* binaryInterpolationEncodedFileName, const char* binaryInterpolationDencodedFileName, size_t fileSize) {
    // Encoding
    auto startEncoded = std::chrono::high_resolution_clock::now();
    std::vector<char> encoded = encode(binaryData);
    auto endEncoded = std::chrono::high_resolution_clock::now();
    auto encodedDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endEncoded - startEncoded);

    // Decoding
    auto startDecoded = std::chrono::high_resolution_clock::now();
    std::vector<char> decoded = decode(encoded);
    auto endDecoded = std::chrono::high_resolution_clock::now();
    auto decodedDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endDecoded - startDecoded);

    // Calculate the throughput for encoding and decoding
    avgEncodedThroughput += binaryData.size() / static_cast<double>(encodedDuration.count()) * 1000; // bytes/sec
    avgThroughputDecoded += decoded.size() / static_cast<double>(decodedDuration.count()) * 1000; // bytes/sec


    // Verify that no data is lost by comparing decoded data with the original data
    bool dataMatches = binaryData == decoded;
    //std::cout << "LZO1 Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

    // Create a binary file from LZO1 encoded data for further verification
    std::ofstream binaryInterpolationOutFile(binaryInterpolationEncodedFileName, std::ios::binary);
    std::ofstream binaryInterpolationDecodedOutFile(binaryInterpolationDencodedFileName, std::ios::binary);
    if (!binaryInterpolationOutFile || !binaryInterpolationDecodedOutFile) {
        std::cerr << "Error: Unable to create the LZO1 encoded or decoded file." << std::endl;
    }

    for (size_t i = 0; i < encoded.size(); ++i) {
        std::bitset<1> code = encoded[i];
        binaryInterpolationOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
    }

    for (size_t i = 0; i < decoded.size(); ++i) {
        char byte = decoded[i];
        binaryInterpolationDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    binaryInterpolationOutFile.close();
    binaryInterpolationDecodedOutFile.close();

    // open the LZO1 encoded file and determine the file size
    avgSizeBytes += getFileSize(binaryInterpolationEncodedFileName);
    avgEncodedTimeMs += encodedDuration.count();
    avgDecodedTimeMs += decodedDuration.count();
    avgCompressionRatio += static_cast<double>(getFileSize(binaryInterpolationEncodedFileName)) / fileSize;
}

void LZO1_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations) {
    // Read the binary file
    std::vector<char> binaryData;
    std::ifstream binaryFile(filename, std::ios::binary);
    if (!binaryFile) {
        std::cerr << "Error: Unable to open the binary file." << std::endl;
    }

    // Determine the file size
    size_t fileSize = getFileSize(filename);

    // Read the binary file
    binaryData.resize(fileSize);
    binaryFile.read(binaryData.data(), fileSize);
    binaryFile.close();

    // Perform encoding and decoding
    for (int i = 0; i < numIterations; ++i) {
        getFileStats(binaryData, "binary_interpolation_encoded.bin", "binary_interpolation_decoded.bin", fileSize);
    }
}

// Setters
void LZO1_Stats::setAvgSizeBytes(double value) {
    avgSizeBytes = value;
}

void LZO1_Stats::setAvgEncodedTimeMs(double value) {
    avgEncodedTimeMs = value;
}

void LZO1_Stats::setAvgDecodedTimeMs(double value) {
    avgDecodedTimeMs = value;
}

void LZO1_Stats::setAvgCompressionRatio(double value) {
    avgCompressionRatio = value;
}

void LZO1_Stats::setAvgPeakMemoryDuringEncoding(size_t value) {
    avgPeakMemoryDuringEncoding = value;
}

void LZO1_Stats::setAvgPeakMemoryDuringDecoding(size_t value) {
    avgPeakMemoryDuringDecoding = value;
}

void LZO1_Stats::setAvgEncodedThroughput(double value) {
    avgEncodedThroughput = value;
}

void LZO1_Stats::setAvgThroughputDecoded(double value) {
    avgThroughputDecoded = value;
}

// Getters
double LZO1_Stats::getAvgSizeBytes() const {
    return avgSizeBytes;
}

double LZO1_Stats::getAvgEncodedTimeMs() const {
    return avgEncodedTimeMs;
}

double LZO1_Stats::getAvgDecodedTimeMs() const {
    return avgDecodedTimeMs;
}

double LZO1_Stats::getAvgCompressionRatio() const {
    return avgCompressionRatio;
}

size_t LZO1_Stats::getAvgPeakMemoryDuringEncoding() const {
    return avgPeakMemoryDuringEncoding;
}

size_t LZO1_Stats::getAvgPeakMemoryDuringDecoding() const {
    return avgPeakMemoryDuringDecoding;
}

double LZO1_Stats::getAvgEncodedThroughput() const {
    return avgEncodedThroughput;
}

double LZO1_Stats::getAvgThroughputDecoded() const {
    return avgThroughputDecoded;
}
