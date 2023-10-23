#include "huffman_stats.h"

// Constructors
Huffman_Stats::Huffman_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

// Functions
void Huffman_Stats::printStats() {
    std::cout << "Huffman: Average size in bytes: " << avgSizeBytes << std::endl;
    std::cout << "Huffman: Average encoded time in ms: " << avgEncodedTimeMs << std::endl;
    std::cout << "Huffman: Average decoded time in ms: " << avgDecodedTimeMs << std::endl;
    std::cout << "Huffman: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "Huffman: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "Huffman: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "Huffman: Average encoded throughput: " << avgEncodedThroughput << std::endl;
    std::cout << "Huffman: Average throughput decoded: " << avgThroughputDecoded << std::endl;
}

void Huffman_Stats::calculateAvgStats(int divisor){
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void Huffman_Stats::getFileStats(const char* filename, const char* huffmanEncodedFileName, const char* huffmanDecodedFileName, size_t fileSize, std::filesystem::path& currentDir){
    std::filesystem::path huffmanEncodedFilePath = currentDir / huffmanEncodedFileName;
    std::filesystem::path huffmanDecodedFilePath = currentDir / huffmanDecodedFileName;

    Huffman h;
    // Perform huffman encoding
    auto startEncodehuffman = std::chrono::high_resolution_clock::now();
    h.encode(filename);
    auto stopEncodehuffman = std::chrono::high_resolution_clock::now();
    auto durationEncodehuffman = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncodehuffman - startEncodehuffman);

    // Perform huffman decoding
    auto startDecodehuffman = std::chrono::high_resolution_clock::now();

    auto stopDecodehuffman = std::chrono::high_resolution_clock::now();
    auto durationDecodehuffman = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecodehuffman - startDecodehuffman);

    // Calculate the peak memory usage
    // avghuffmanStats.setAvgPeakMemoryDuringDecoding(avghuffmanStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
    // avghuffmanStats.setAvgPeakMemoryDuringEncoding(avghuffmanStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

    // Calculate the throughput for encoding and decoding


    // Verify that no data is lost by comparing decoded data with the original data
    // bool huffmanDataMatches = binaryData == huffmanDecoded;
    // std::cout << "Huffman Data Matches: " << (huffmanDataMatches ? "Yes" : "No") << std::endl;

    // Create a binary file from huffman encoded data for further verification
    // open the huffman encoded file and determine the file size
    // avgSizeBytes += getFileSize(huffmanEncodedFileName, currentDir);
    // avgEncodedTimeMs += durationEncodehuffman.count()*1000000;
    // avgDecodedTimeMs += durationDecodehuffman.count()*1000000;
    // avgCompressionRatio += (static_cast<double>(getFileSize(huffmanEncodedFileName, currentDir)) / fileSize);
    // avgEncodedThroughput += binaryData.size() / static_cast<double>(durationEncodehuffman.count()) * 1000000000; // bytes/sec
    // avgThroughputDecoded += huffmanDecoded.size() / static_cast<double>(durationDecodehuffman.count()) * 1000000000; // bytes/sec
}

 void Huffman_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir) {
    //std::cout << "Compressing " << filename << " using huffman" << std::endl;
    const char* huffmanEncodedFilename = "huffman_encoded.bin";
    const char* huffmanDecodedFilename = "huffman_decoded.bin";

     Huffman_Stats avghuffmanStats;

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

        // Perform huffman encoding and decoding
        avghuffmanStats.getFileStats(filename, huffmanEncodedFilename, huffmanDecodedFilename, fileSize, currentDir);
    }

    // Calculate the average stats for the current file
    avghuffmanStats.calculateAvgStats(numIterations);

    avgSizeBytes += avghuffmanStats.getAvgSizeBytes();
    avgEncodedTimeMs += avghuffmanStats.getAvgEncodedTimeMs();
    avgDecodedTimeMs += avghuffmanStats.getAvgDecodedTimeMs();
    avgCompressionRatio += avghuffmanStats.getAvgCompressionRatio();
    avgPeakMemoryDuringEncoding += avghuffmanStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avghuffmanStats.getAvgPeakMemoryDuringDecoding();
    avgEncodedThroughput += avghuffmanStats.getAvgEncodedThroughput();
    avgThroughputDecoded += avghuffmanStats.getAvgThroughputDecoded();
 }

// Setters
void Huffman_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void Huffman_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void Huffman_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void Huffman_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void Huffman_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void Huffman_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void Huffman_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void Huffman_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double Huffman_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double Huffman_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double Huffman_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double Huffman_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t Huffman_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t Huffman_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double Huffman_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double Huffman_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }