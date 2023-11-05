#include "huffman_stats.h"

// Constructors
Huffman_Stats::Huffman_Stats() :
    avgSizeBytes(0.0), average_time_encoded_in_ns (0.0), average_time_encoded_in_ns (0.0),
    average_compression_ratio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    average_encoded_throughput(0.0), average_decoded_throughput(0.0) {}

// Functions
void Huffman_Stats::Print_Stats() {
    std::cout << "Huffman: Average size in bytes: " << avgSizeBytes << '\n';
    std::cout << "Huffman: Average encoded time in ms: " << average_time_encoded_in_ns  << '\n';
    std::cout << "Huffman: Average decoded time in ms: " << average_time_encoded_in_ns  << '\n';
    std::cout << "Huffman: Average compression ratio: " << average_compression_ratio << '\n';
    std::cout << "Huffman: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << '\n';
    std::cout << "Huffman: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << '\n';
    std::cout << "Huffman: Average encoded throughput: " << average_encoded_throughput << '\n';
    std::cout << "Huffman: Average throughput decoded: " << average_decoded_throughput << '\n';
}

void Huffman_Stats::Calculate_Cumulative_Average_Stats_For_Directory(int divisor){
    avgSizeBytes /= divisor;
    average_time_encoded_in_ns  /= divisor;
    average_time_encoded_in_ns  /= divisor;
    average_compression_ratio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    average_encoded_throughput /= divisor;
    average_decoded_throughput /= divisor;
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
    // std::cout << "Huffman Data Matches: " << (huffmanDataMatches ? "Yes" : "No") << '\n';

    // Create a binary file from huffman encoded data for further verification
    // open the huffman encoded file and determine the file size
    // avgSizeBytes += getFileSize(huffmanEncodedFileName, currentDir);
    // average_time_encoded_in_ns  += durationEncodehuffman.count()*1000000;
    // average_time_encoded_in_ns  += durationDecodehuffman.count()*1000000;
    // average_compression_ratio += (static_cast<double>(getFileSize(huffmanEncodedFileName, currentDir)) / fileSize);
    // average_encoded_throughput += binaryData.size() / static_cast<double>(durationEncodehuffman.count()) * 1000000000; // bytes/sec
    // average_decoded_throughput += huffmanDecoded.size() / static_cast<double>(durationDecodehuffman.count()) * 1000000000; // bytes/sec
}

 void Huffman_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir) {
    //std::cout << "Compressing " << filename << " using huffman" << '\n';
    const char* huffmanEncodedFilename = "huffman_encoded.bin";
    const char* huffmanDecodedFilename = "huffman_decoded.bin";

     Huffman_Stats avghuffmanStats;

    for (int i = 0; i < numIterations; ++i) {
        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << '\n';
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
    avghuffmanStats.Calculate_Cumulative_Average_Stats_For_Directory(numIterations);

    avgSizeBytes += avghuffmanStats.getAvgSizeBytes();
    average_time_encoded_in_ns  += avghuffmanStats.getaverage_time_encoded_in_ns ();
    average_time_encoded_in_ns  += avghuffmanStats.getaverage_time_encoded_in_ns ();
    average_compression_ratio += avghuffmanStats.getaverage_compression_ratio();
    avgPeakMemoryDuringEncoding += avghuffmanStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avghuffmanStats.getAvgPeakMemoryDuringDecoding();
    average_encoded_throughput += avghuffmanStats.getaverage_encoded_throughput();
    average_decoded_throughput += avghuffmanStats.getaverage_decoded_throughput();
 }

// Setters
void Huffman_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void Huffman_Stats::setaverage_time_encoded_in_ns (double value) { average_time_encoded_in_ns  = value; }
void Huffman_Stats::setaverage_time_encoded_in_ns (double value) { average_time_encoded_in_ns  = value; }
void Huffman_Stats::setaverage_compression_ratio(double value) { average_compression_ratio = value; }
void Huffman_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void Huffman_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void Huffman_Stats::setaverage_encoded_throughput(double value) { average_encoded_throughput = value; }
void Huffman_Stats::setaverage_decoded_throughput(double value) { average_decoded_throughput = value; }

// Getters
double Huffman_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double Huffman_Stats::getaverage_time_encoded_in_ns () const { return average_time_encoded_in_ns ; }
double Huffman_Stats::getaverage_time_encoded_in_ns () const { return average_time_encoded_in_ns ; }
double Huffman_Stats::getaverage_compression_ratio() const { return average_compression_ratio; }
size_t Huffman_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t Huffman_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double Huffman_Stats::getaverage_encoded_throughput() const { return average_encoded_throughput; }
double Huffman_Stats::getaverage_decoded_throughput() const { return average_decoded_throughput; }