#include "control_class.h"

// Constructors
Control_Stats::Control_Stats() {
    avgSizeBytes = 0;
    avgEncodedTimeMs = 0;
    avgDecodedTimeMs = 0;
    avgCompressionRatio = 0;
    avgPeakMemoryDuringEncoding = 0;
    avgPeakMemoryDuringDecoding = 0;
    avgEncodedThroughput = 0;
    avgThroughputDecoded = 0;
}

Control_Stats::Control_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs,
        double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding,
        size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) {
    this->avgSizeBytes = avgSizeBytes;
    this->avgEncodedTimeMs = avgEncodedTimeMs;
    this->avgDecodedTimeMs = avgDecodedTimeMs;
    this->avgCompressionRatio = avgCompressionRatio;
    this->avgPeakMemoryDuringEncoding = avgPeakMemoryDuringEncoding;
    this->avgPeakMemoryDuringDecoding = avgPeakMemoryDuringDecoding;
    this->avgEncodedThroughput = avgEncodedThroughput;
    this->avgThroughputDecoded = avgThroughputDecoded;
}

std::vector<char> Control_Stats::encode(const std::vector<char>& input) {
    std::vector<char> encoded;

    for(int i = 0; i < input.size(); i++){
        encoded.push_back(input[i]);
    }

    return encoded;

}

std::vector<char> Control_Stats::decode(const std::vector<char>& input) {
    std::vector<char> decoded;

    for(int i = 0; i < input.size(); i++){
        decoded.push_back(input[i]);
    }

    return decoded;
}

//functions
void Control_Stats::printStats() {
    std::cout << "Control: Average size (bytes): " << avgSizeBytes << "\n";
    std::cout << "Control: Average encoded time (ms): " << avgEncodedTimeMs << "\n";
    std::cout << "Control: Average decoded time (ms): " << avgDecodedTimeMs << "\n";
    std::cout << "Control: Average compression ratio: " << avgCompressionRatio << "\n";
    std::cout << "Control: Average peak memory during encoding (bytes): " << avgPeakMemoryDuringEncoding << "\n";
    std::cout << "Control: Average peak memory during decoding (bytes): " << avgPeakMemoryDuringDecoding << "\n";
    std::cout << "Control: Average encoded throughput (MB/s): " << avgEncodedThroughput << "\n";
    std::cout << "Control: Average decoded throughput (MB/s): " << avgThroughputDecoded << "\n";
}

void Control_Stats::calculateAvgStats(int divisors) {
    avgSizeBytes /= divisors;
    avgEncodedTimeMs /= divisors;
    avgDecodedTimeMs /= divisors;
    avgCompressionRatio /= divisors;
    avgPeakMemoryDuringEncoding /= divisors;
    avgPeakMemoryDuringDecoding /= divisors;
    avgEncodedThroughput /= divisors;
    avgThroughputDecoded /= divisors;
}

void Control_Stats::getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* decodedFilename, size_t fileSize, std::filesystem::path& currentDir){
    auto startEncod = std::chrono::high_resolution_clock::now();
    std::vector<char> encoded = encode(binaryData);
    auto stopEncode = std::chrono::high_resolution_clock::now();
    auto durationEncode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncode - startEncod);

    // decoding
    auto startDecode = std::chrono::high_resolution_clock::now();
    std::vector<char> decoded = decode(encoded);
    auto stopDecode = std::chrono::high_resolution_clock::now();
    auto durationDecode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecode - startDecode);

    // Calculate the average size in bytes
    avgSizeBytes += fileSize;
    avgEncodedTimeMs += durationEncode.count() * 1000000;
    avgDecodedTimeMs += durationDecode.count() * 1000000;
    avgCompressionRatio += static_cast<double>(getFileSize(encodedFilename))/fileSize;
    avgEncodedThroughput += static_cast<double>(fileSize) / durationEncode.count() * 1000000000; // bytes per second
    avgThroughputDecoded += static_cast<double>(fileSize) / durationDecode.count() * 1000000000; // bytes per second
}

void Control_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir){
    std::string encodedFilename =  std::string(filename).erase(std::string(filename).size()-7,6) + ".control_encoded_bin";
    std::string dencodedFilename = std::string(filename).erase(std::string(filename).size()-7,6)  + ".run_length_decoded_bin";

    Control_Stats localStats;

    for (int i = 0; i < numIterations; ++i) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cout << "Error" << std::endl;
            return;  // Error handling
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        localStats.getFileStats(binaryData, encodedFilename.c_str(), dencodedFilename.c_str(), fileSize, currentDir);
    }

    // Calculate the average stats for the current file
    localStats.calculateAvgStats(numIterations);

    avgSizeBytes = localStats.avgSizeBytes;
    avgEncodedTimeMs = localStats.avgEncodedTimeMs;
    avgDecodedTimeMs = localStats.avgDecodedTimeMs;
    avgCompressionRatio = localStats.avgCompressionRatio;
    avgPeakMemoryDuringEncoding = localStats.avgPeakMemoryDuringEncoding;
    avgPeakMemoryDuringDecoding = localStats.avgPeakMemoryDuringDecoding;
    avgEncodedThroughput = localStats.avgEncodedThroughput;
    avgThroughputDecoded = localStats.avgThroughputDecoded;
}

// Setters
void Control_Stats::setAvgSizeBytes(double value) {
    avgSizeBytes = value;
}

void Control_Stats::setAvgEncodedTimeMs(double value) {
    avgEncodedTimeMs = value;
}

void Control_Stats::setAvgDecodedTimeMs(double value) {
    avgDecodedTimeMs = value;
}

void Control_Stats::setAvgCompressionRatio(double value) {
    avgCompressionRatio = value;
}

void Control_Stats::setAvgPeakMemoryDuringEncoding(size_t value) {
    avgPeakMemoryDuringEncoding = value;
}

void Control_Stats::setAvgPeakMemoryDuringDecoding(size_t value) {
    avgPeakMemoryDuringDecoding = value;
}

void Control_Stats::setAvgEncodedThroughput(double value) {
    avgEncodedThroughput = value;
}

void Control_Stats::setAvgThroughputDecoded(double value) {
    avgThroughputDecoded = value;
}

// Getters
double Control_Stats::getAvgSizeBytes() const {
    return avgSizeBytes;
}

double Control_Stats::getAvgEncodedTimeMs() const {
    return avgEncodedTimeMs;
}

double Control_Stats::getAvgDecodedTimeMs() const {
    return avgDecodedTimeMs;
}

double Control_Stats::getAvgCompressionRatio() const {
    return avgCompressionRatio;
}

size_t Control_Stats::getAvgPeakMemoryDuringEncoding() const {
    return avgPeakMemoryDuringEncoding;
}

size_t Control_Stats::getAvgPeakMemoryDuringDecoding() const {
    return avgPeakMemoryDuringDecoding;
}

double Control_Stats::getAvgEncodedThroughput() const {
    return avgEncodedThroughput;
}

double Control_Stats::getAvgThroughputDecoded() const {
    return avgThroughputDecoded;
}
