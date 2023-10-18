#include "rlr_class.h"

//Constructors
RLR_Stats::RLR_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

RLR_Stats::RLR_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

// Run-length encoding
// Time Complexity:
// Best, Average, Worst: O(n)
// Space Complexity:
// Best, Average, Worst: O(n)
std::vector<std::pair<char, int>> RLR_Stats::runLengthEncode(const std::vector<char>& input){
        if (input.empty()) {
        return {};
    }

    std::vector<std::pair<char, int>> runLengthEncoded;
    runLengthEncoded.reserve(input.size()); // Reserve the maximum potential size.

    char currentChar = input[0];
    int runLength = 1;

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == currentChar && runLength < 255) {
            runLength++;
        } else {
            runLengthEncoded.emplace_back(currentChar, runLength);
            currentChar = input[i];
            runLength = 1;
        }
    }

    // After the loop, don't forget to add the last run.
    runLengthEncoded.emplace_back(currentChar, runLength);

    runLengthEncoded.shrink_to_fit(); // Reduce capacity to fit the actual size.

    return runLengthEncoded;
}

// Run-length decoding
// Time Complexity:
// Best, Average, Worst: O(n)
// Space Complexity:
// Best, Average, Worst: O(n)
std::vector<char> RLR_Stats::runLengthDecode(const std::vector<std::pair<char, int>>& input) {
    std::vector<char> runLengthDecoded;
    size_t totalLength = 0;

    // Calculate the total length to reserve capacity.
    for (const auto& pair : input) {
        totalLength += pair.second;
    }
    runLengthDecoded.reserve(totalLength);

    for (const auto& pair : input) {
        std::fill_n(std::back_inserter(runLengthDecoded), pair.second, pair.first);
    }

    return runLengthDecoded;
}

//functions
void RLR_Stats::printStats() {
    std::cout << "RLR: Average size in bytes: " << avgSizeBytes << std::endl;
    std::cout << "RLR: Average encoded time in ms: " << avgEncodedTimeMs << std::endl;
    std::cout << "RLR: Average decoded time in ms: " << avgDecodedTimeMs << std::endl;
    std::cout << "RLR: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "RLR: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "RLR: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "RLR: Average encoded throughput: " << avgEncodedThroughput << std::endl;
    std::cout << "RLR: Average throughput decoded: " << avgThroughputDecoded << std::endl;
}

void RLR_Stats::calculateAvgStats(int divisor){
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void RLR_Stats::getFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize){
        // Perform run-length encoding
        auto startEncodRunLength = std::chrono::high_resolution_clock::now();
        auto runLengthEncoded = runLengthEncode(binaryData);
        auto stopEncodRunLength = std::chrono::high_resolution_clock::now();
        auto durationEncodRunLength = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodRunLength - startEncodRunLength);

        // Perform run-length decoding
        auto startDecodRunLength = std::chrono::high_resolution_clock::now();
        auto runLengthDecoded = runLengthDecode(runLengthEncoded);
        auto stopDecodRunLength = std::chrono::high_resolution_clock::now();
        auto durationDecodRunLength = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodRunLength - startDecodRunLength);

        // Calculate the peak memory usage
        // avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        // avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgEncodedThroughput += static_cast<double>(fileSize) / durationEncodRunLength.count() * 1000; // bytes per second
        avgThroughputDecoded += static_cast<double>(fileSize) / durationDecodRunLength.count() * 1000; // bytes per second

        // Verify that no data is lost by comparing decoded data with the original data
        bool dataMatches = binaryData == runLengthDecoded;
        std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

        // Write the run-length encoded and decoded files
        std::ofstream runLengthOutFile(runLengthFilename, std::ios::binary);
        std::ofstream runLengthDecodedOutFile(runLengthDecodedFilename, std::ios::binary);
        if (!runLengthOutFile || !runLengthDecodedOutFile) {
            std::cerr << "Error: Unable to create the run-length encoded or decoded file." << std::endl;
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
        avgSizeBytes += getFileSize(runLengthFilename);
        avgEncodedTimeMs += durationEncodRunLength.count();
        avgDecodedTimeMs += durationDecodRunLength.count();
        avgCompressionRatio += static_cast<double>(getFileSize(runLengthFilename)) / fileSize;

}

 void RLR_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations) {
    std::cout << "Compressing " << filename << " using RLR " << std::endl;
    const char* runLengthFilename = "run_length_encoded.bin";
    const char* runLengthDecodedFilename = "run_length_decoded.bin";

    RLR_Stats avgRLRStats;

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

        // Perform run-length encoding and decoding
        avgRLRStats.getFileStats(binaryData, runLengthFilename, runLengthDecodedFilename, fileSize);
    }

    // Calculate the average stats for the current file
    avgRLRStats.calculateAvgStats(numIterations);

    avgSizeBytes += avgRLRStats.getAvgSizeBytes();
    avgEncodedTimeMs += avgRLRStats.getAvgEncodedTimeMs();
    avgDecodedTimeMs += avgRLRStats.getAvgDecodedTimeMs();
    avgCompressionRatio += avgRLRStats.getAvgCompressionRatio();
    avgPeakMemoryDuringEncoding += avgRLRStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avgRLRStats.getAvgPeakMemoryDuringDecoding();
    avgEncodedThroughput += avgRLRStats.getAvgEncodedThroughput();
    avgThroughputDecoded += avgRLRStats.getAvgThroughputDecoded();

 }

// Setters
void RLR_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void RLR_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void RLR_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void RLR_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void RLR_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void RLR_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void RLR_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void RLR_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double RLR_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double RLR_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double RLR_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double RLR_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t RLR_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t RLR_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double RLR_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double RLR_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }
