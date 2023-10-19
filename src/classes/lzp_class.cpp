#include "lzp_class.h"

//Constructors
LZP_Stats::LZP_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

LZP_Stats::LZP_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

// Lempel-Ziv Parallel (LZP) encoding
// Time Complexity:
// Best, Average:O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst: O(n)

std::vector<int> LZP_Stats::lzpEncode(const std::vector<char>& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzpEncoded;
    lzpEncoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            current.pop_back();  // Remove the last character
            lzpEncoded.push_back(dictionary[current]);
            dictionary[current + c] = dictionary.size();
            current = c;
        }
    }

    if (!current.empty()) {
        lzpEncoded.push_back(dictionary[current]);
    }

    return lzpEncoded;
}

// Lempel-Ziv Parallel (LZP) decoding
// Time Complexity:
// Best, Average:O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst: O(n)
std::vector<char> LZP_Stats::lzpDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzpDecoded;
    lzpDecoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzpDecoded.insert(lzpDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        size_t code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZP decoding error: Invalid code.");
        }

        lzpDecoded.insert(lzpDecoded.end(), entry.begin(), entry.end());

        dictionary[dictionary.size()] = current + entry[0];
        current = entry;
    }

    return lzpDecoded;
}


// Functions
void LZP_Stats::printStats() {
    std::cout << "LZP: Average size in bytes: " << avgSizeBytes << std::endl;
    std::cout << "LZP: Average encoded time in ms: " << avgEncodedTimeMs << std::endl;
    std::cout << "LZP: Average decoded time in ms: " << avgDecodedTimeMs << std::endl;
    std::cout << "LZP: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "LZP: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "LZP: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "LZP: Average encoded throughput: " << avgEncodedThroughput << std::endl;
    std::cout << "LZP: Average throughput decoded: " << avgThroughputDecoded << std::endl;
}

void LZP_Stats::calculateAvgStats(int divisor){
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void LZP_Stats::getFileStats(std::vector<char> &binaryData, const char* lzpEncodedFileName, const char* lzpDecodedFileName, size_t fileSize){
        auto startEncodeLzp = std::chrono::high_resolution_clock::now();
        std::vector<int> lzpEncoded = lzpEncode(binaryData);
        auto stopEncodeLzp = std::chrono::high_resolution_clock::now();
        auto durationEncodeLzp = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodeLzp - startEncodeLzp);

        // Perform LZP decoding
        auto startDecodeLzp = std::chrono::high_resolution_clock::now();
        std::vector<char> lzpDecoded = lzpDecode(lzpEncoded);
        auto stopDecodeLzp = std::chrono::high_resolution_clock::now();
        auto durationDecodeLzp = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodeLzp - startDecodeLzp);

        // Calculate the peak memory usage
        // avgLZPStats.setAvgPeakMemoryDuringDecoding(avgLZPStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        // avgLZPStats.setAvgPeakMemoryDuringEncoding(avgLZPStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgEncodedThroughput += binaryData.size() / static_cast<double>(durationEncodeLzp.count()) * 1000; // bytes/sec
        avgThroughputDecoded += lzpDecoded.size() / static_cast<double>(durationDecodeLzp.count()) * 1000; // bytes/sec

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzpDataMatches = binaryData == lzpDecoded;
        std::cout << "LZP Data Matches: " << (lzpDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZP encoded data for further verification
        std::ofstream lzpOutFile(lzpEncodedFileName, std::ios::binary);
        std::ofstream lzpDecodedOutFile(lzpDecodedFileName, std::ios::binary);
        if (!lzpOutFile || !lzpDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZP encoded or decoded file." << std::endl;
        }

        for (size_t i = 0; i < lzpEncoded.size(); ++i) {
            int code = lzpEncoded[i];
            lzpOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
        }

        for (size_t i = 0; i < lzpDecoded.size(); ++i) {
            char byte = lzpDecoded[i];
            lzpDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        lzpOutFile.close();
        lzpDecodedOutFile.close();

        // open the LZP encoded file and determine the file size
        avgSizeBytes += getFileSize(lzpEncodedFileName);
        avgEncodedTimeMs += durationEncodeLzp.count();
        avgDecodedTimeMs += durationDecodeLzp.count();
        avgCompressionRatio += static_cast<double>(getFileSize(lzpEncodedFileName)) / fileSize;
}

 void LZP_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations) {
    std::cout << "Compressing " << filename << " using LZP" << std::endl;
    const char* lzpEncodedFilename = "lzp_encoded.bin";
    const char* lzpDecodedFilename = "lzp_decoded.bin";

     LZP_Stats avglzpStats;

        for (int i = 0; i < numIterations; ++i) {
        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << std::endl;
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        // Perform lzp encoding and decoding
        avglzpStats.getFileStats(binaryData, lzpEncodedFilename, lzpDecodedFilename, fileSize);
    }

    // Calculate the average stats for the current file
    avglzpStats.calculateAvgStats(numIterations);

    avgSizeBytes += avglzpStats.getAvgSizeBytes();
    avgEncodedTimeMs += avglzpStats.getAvgEncodedTimeMs();
    avgDecodedTimeMs += avglzpStats.getAvgDecodedTimeMs();
    avgCompressionRatio += avglzpStats.getAvgCompressionRatio();
    avgPeakMemoryDuringEncoding += avglzpStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avglzpStats.getAvgPeakMemoryDuringDecoding();
    avgEncodedThroughput += avglzpStats.getAvgEncodedThroughput();
    avgThroughputDecoded += avglzpStats.getAvgThroughputDecoded();
 }

// Setters
void LZP_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void LZP_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void LZP_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void LZP_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void LZP_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void LZP_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void LZP_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void LZP_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double LZP_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double LZP_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double LZP_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double LZP_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t LZP_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t LZP_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double LZP_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double LZP_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }