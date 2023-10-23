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


std::vector<std::pair<char, uint8_t>> RLR_Stats::encodeRow(const std::vector<char>& input, int row, int data_type_size) {
    std::vector<std::pair<char, uint8_t>> encodedRow;

    // Ensure the segment doesn't go beyond the input's bounds
    size_t end = std::min(static_cast<size_t>(row + data_type_size), static_cast<size_t>(input.size()));

    if (row >= end) {
        return encodedRow; // Return empty result for empty input segment
    }

    char currentChar = input[row];
    uint8_t runLength = 1;

    for (size_t i = row + 1; i < end; ++i) {
        if (input[i] == currentChar && runLength < 255) {
            runLength++;
        } else {
            encodedRow.emplace_back(currentChar, runLength);
            currentChar = input[i];
            runLength = 1;
        }
    }

    encodedRow.emplace_back(currentChar, runLength);  // Add the last run for this row.
    return encodedRow;
}

std::vector<std::pair<char, uint8_t>> RLR_Stats::encode(const std::vector<char>& input, int data_type_size) {
    if (input.empty() || data_type_size <= 0) {
        return {};
    }

    std::vector<std::pair<char, uint8_t>> encoded;
    for (size_t i = 0; i < input.size(); i += data_type_size) {
        auto rowResult = encodeRow(input, i, data_type_size);
        encoded.insert(encoded.end(), rowResult.begin(), rowResult.end());
    }

    return encoded;
}

std::vector<char> RLR_Stats::decodeRow(const std::vector<std::pair<char, uint8_t>>& input, int row, int data_type_size) {
    std::vector<char> decodedRow;

    // Calculate the starting position and ending position based on row and data_type_size
    int start = row;
    int end = std::min(start + data_type_size, static_cast<int>(input.size()));

    for (int i = start; i < end; ++i) {
        decodedRow.insert(decodedRow.end(), input[i].second, input[i].first);
    }

    return decodedRow;
}


std::vector<char> RLR_Stats::decode(const std::vector<std::pair<char, uint8_t>>& input, int data_type_size) {
    std::vector<char> decoded;

    // Estimate the total length and reserve capacity.
    size_t totalLength = 0;
    for (const auto& pair : input) {
        totalLength += pair.second;
    }
    decoded.reserve(totalLength);

    for (size_t i = 0; i < input.size(); i += data_type_size) {
        auto rowResult = decodeRow(input, i, data_type_size);
        decoded.insert(decoded.end(), rowResult.begin(), rowResult.end());
    }

    return decoded;
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

void RLR_Stats::getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* dencodedFilename, size_t fileSize, std::filesystem::path& currentDir){
    //find the .geometa file
    std::filesystem::path geometaFile;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(currentDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".geometa") {
            geometaFile = entry.path();
            break; // Optional: stop after finding the first .geometa file
        }
    }
    //read the .geometa file which is a json file
    // Step 2: Read the .geometa file which is a json file
    std::ifstream inFile(geometaFile);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open the .geometa file." << std::endl;
    }

    std::string line;
    std::string datatype;
    while (std::getline(inFile, line)) {
        // Assuming the line format is "key": "value"
        size_t keyStart = line.find("\"store_sc_type\":");
        if (keyStart != std::string::npos) {
            size_t valueStart = line.find('"', keyStart + 16); // 16 = length of "store_sc_type":
            if (valueStart != std::string::npos) {
                size_t valueEnd = line.find('"', valueStart + 1);
                if (valueEnd != std::string::npos) {
                    datatype = line.substr(valueStart + 1, valueEnd - valueStart - 1);
                    //std::cout << datatype << "\n";
                    break; // Optional: stop after finding the first match
                }
            }
        }
    }

    inFile.close();

    int datatype_size = 0;
    //this will be a case statement
    if (datatype == "int16" || datatype == "uint16") {
        datatype_size = 16;
    }


    // Perform run-length encoding
    auto startEncod = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<char, uint8_t>> encoded = encode(binaryData, datatype_size);
    auto stopEncode = std::chrono::high_resolution_clock::now();
    auto durationEncode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncode - startEncod);

    // Perform run-length decoding
    auto startDecode = std::chrono::high_resolution_clock::now();
    std::vector<char> decoded = decode(encoded, datatype_size);
    auto stopDecode = std::chrono::high_resolution_clock::now();
    auto durationDecode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecode - startDecode);

    // Calculate the peak memory usage
    // avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
    // avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

    // Verify that no data is lost by comparing decoded data with the original data
    bool dataMatches = binaryData == decoded;
    assert(dataMatches);
    //std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

    // Write the run-length encoded and decoded files
    std::ofstream encodedOutFile(encodedFilename, std::ios::binary);
    std::ofstream decodedOutFile(dencodedFilename, std::ios::binary);
    if (!encodedOutFile || !decodedOutFile) {
        std::cerr << "Error: Unable to create the run-length encoded or decoded file." << std::endl;
    }

    // Writing encoded data to file
    for (const auto& pair : encoded) {
    char byte = pair.first;
    uint8_t count = pair.second;

    encodedOutFile.write(&byte, sizeof(char));
    encodedOutFile.write(reinterpret_cast<const char*>(&count), sizeof(uint8_t));
    }

    // Writing decoded data to file
    decodedOutFile.write(decoded.data(), decoded.size());

    encodedOutFile.close();
    decodedOutFile.close();

    // open the run-length encoded file and determine the file size
    avgSizeBytes += getFileSize(encodedFilename);
    avgEncodedTimeMs += durationEncode.count() * 1000000;
    avgDecodedTimeMs += durationDecode.count() * 1000000;
    avgCompressionRatio += static_cast<double>(getFileSize(encodedFilename))/fileSize;
    avgEncodedThroughput += static_cast<double>(fileSize) / durationEncode.count() * 1000000000; // bytes per second
    avgThroughputDecoded += static_cast<double>(fileSize) / durationDecode.count() * 1000000000; // bytes per second

}


void RLR_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir) {
    std::string encodedFilename =  std::string(filename).erase(std::string(filename).size()-7,6) + ".run_length_encoded_bin";
    std::string dencodedFilename = std::string(filename).erase(std::string(filename).size()-7,6)  + ".run_length_decoded_bin";

    RLR_Stats localStats;

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
