#include "control_class.h"

// Constructors
Control_Stats::Control_Stats() {
    avgSizeBytes = 0;
    average_time_encoded_in_s  = 0;
    average_time_decoded_in_s  = 0;
    average_compression_ratio = 0;
    avgPeakMemoryDuringEncoding = 0;
    avgPeakMemoryDuringDecoding = 0;
    average_encoded_throughput = 0;
    average_decoded_throughput = 0;
}

Control_Stats::Control_Stats(double avgSizeBytes, double average_time_encoded_in_s , double average_time_decoded_in_s ,
        double average_compression_ratio, size_t avgPeakMemoryDuringEncoding,
        size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput) {
    this->avgSizeBytes = avgSizeBytes;
    this->average_time_encoded_in_s  = average_time_encoded_in_s ;
    this->average_time_decoded_in_s  = average_time_decoded_in_s ;
    this->average_compression_ratio = average_compression_ratio;
    this->avgPeakMemoryDuringEncoding = avgPeakMemoryDuringEncoding;
    this->avgPeakMemoryDuringDecoding = avgPeakMemoryDuringDecoding;
    this->average_encoded_throughput = average_encoded_throughput;
    this->average_decoded_throughput = average_decoded_throughput;
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
void Control_Stats::Print_Stats() {
    std::cout << "Control: Average size (bytes): " << avgSizeBytes << "\n";
    std::cout << "Control: Average encoded time (ms): " << average_time_encoded_in_s  << "\n";
    std::cout << "Control: Average decoded time (ms): " << average_time_decoded_in_s  << "\n";
    std::cout << "Control: Average compression ratio: " << average_compression_ratio << "\n";
    std::cout << "Control: Average peak memory during encoding (bytes): " << avgPeakMemoryDuringEncoding << "\n";
    std::cout << "Control: Average peak memory during decoding (bytes): " << avgPeakMemoryDuringDecoding << "\n";
    std::cout << "Control: Average encoded throughput (MB/s): " << average_encoded_throughput << "\n";
    std::cout << "Control: Average decoded throughput (MB/s): " << average_decoded_throughput << "\n";
}

void Control_Stats::Calculate_Cumulative_Average_Stats_For_Directory(int divisors) {
    avgSizeBytes /= divisors;
    average_time_encoded_in_s  /= divisors;
    average_time_decoded_in_s  /= divisors;
    average_compression_ratio /= divisors;
    avgPeakMemoryDuringEncoding /= divisors;
    avgPeakMemoryDuringDecoding /= divisors;
    average_encoded_throughput /= divisors;
    average_decoded_throughput /= divisors;
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
    average_time_encoded_in_s  += durationEncode.count() * 1000000;
    average_time_decoded_in_s  += durationDecode.count() * 1000000;
    average_compression_ratio += static_cast<double>(fileSize)/fileSize;
    average_encoded_throughput += static_cast<double>(fileSize) / durationEncode.count() * 1000000000; // bytes per second
    average_decoded_throughput += static_cast<double>(fileSize) / durationDecode.count() * 1000000000; // bytes per second
}

void Control_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir){
    std::string encodedFilename =  std::string(filename).erase(std::string(filename).size()-7,6) + ".control_encoded_bin";
    std::string decodedFilename = std::string(filename).erase(std::string(filename).size()-7,6)  + ".conrol_decoded_bin";

    Control_Stats localStats;

    for (int i = 0; i < numIterations; ++i) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cout << "Error" << "\n";
            return;  // Error handling
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        localStats.getFileStats(binaryData, encodedFilename.c_str(), decodedFilename.c_str(), fileSize, currentDir);
    }

    // Calculate the average stats for the current file
    localStats.Calculate_Cumulative_Average_Stats_For_Directory(numIterations);

    avgSizeBytes = localStats.avgSizeBytes;
    average_time_encoded_in_s  = localStats.average_time_encoded_in_s ;
    average_time_decoded_in_s  = localStats.average_time_decoded_in_s ;
    average_compression_ratio = localStats.average_compression_ratio;
    avgPeakMemoryDuringEncoding = localStats.avgPeakMemoryDuringEncoding;
    avgPeakMemoryDuringDecoding = localStats.avgPeakMemoryDuringDecoding;
    average_encoded_throughput = localStats.average_encoded_throughput;
    average_decoded_throughput = localStats.average_decoded_throughput;
}

// Setters
void Control_Stats::setAvgSizeBytes(double value) {
    avgSizeBytes = value;
}

void Control_Stats::setaverage_time_encoded_in_s (double value) {
    average_time_encoded_in_s  = value;
}

void Control_Stats::setaverage_time_decoded_in_s (double value) {
    average_time_decoded_in_s  = value;
}

void Control_Stats::setaverage_compression_ratio(double value) {
    average_compression_ratio = value;
}

void Control_Stats::setAvgPeakMemoryDuringEncoding(size_t value) {
    avgPeakMemoryDuringEncoding = value;
}

void Control_Stats::setAvgPeakMemoryDuringDecoding(size_t value) {
    avgPeakMemoryDuringDecoding = value;
}

void Control_Stats::setaverage_encoded_throughput(double value) {
    average_encoded_throughput = value;
}

void Control_Stats::setaverage_decoded_throughput(double value) {
    average_decoded_throughput = value;
}

// Getters
double Control_Stats::getAvgSizeBytes() const {
    return avgSizeBytes;
}

double Control_Stats::getaverage_time_encoded_in_s () const {
    return average_time_encoded_in_s ;
}

double Control_Stats::getaverage_time_decoded_in_s () const {
    return average_time_decoded_in_s ;
}

double Control_Stats::getaverage_compression_ratio() const {
    return average_compression_ratio;
}

size_t Control_Stats::getAvgPeakMemoryDuringEncoding() const {
    return avgPeakMemoryDuringEncoding;
}

size_t Control_Stats::getAvgPeakMemoryDuringDecoding() const {
    return avgPeakMemoryDuringDecoding;
}

double Control_Stats::getaverage_encoded_throughput() const {
    return average_encoded_throughput;
}

double Control_Stats::getaverage_decoded_throughput() const {
    return average_decoded_throughput;
}
