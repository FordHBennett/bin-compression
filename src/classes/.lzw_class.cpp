#include "lzw_class.h"

//Constructors
LZW_Stats::LZW_Stats() :
    avgSizeBytes(0.0), average_time_encoded_in_s (0.0), average_time_decoded_in_s (0.0),
    average_compression_ratio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    average_encoded_throughput(0.0), average_decoded_throughput(0.0) {}

LZW_Stats::LZW_Stats(double avgSizeBytes, double average_time_encoded_in_s , double average_time_decoded_in_s , double average_compression_ratio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput) :
    avgSizeBytes(avgSizeBytes), average_time_encoded_in_s (average_time_encoded_in_s ), average_time_decoded_in_s (average_time_decoded_in_s ),
    average_compression_ratio(average_compression_ratio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    average_encoded_throughput(average_encoded_throughput), average_decoded_throughput(average_decoded_throughput) {}

// Space Complexity:
// Best: O(n) for completely repetitive data.
// Average:O(n) for the dictionary plus the output.
// Worst:O(n) for the dictionary plus the output.
std::vector<int> LZW_Stats::lzwEncode(const std::vector<char>& input) {
    const int MAX_DICT_SIZE = 4096/2;  // Example limit
    // std::unordered_map<std::string, int> dictionary;
    std::map<std::string, int> dictionary;
    std::vector<int> lzwEncoded;
    lzwEncoded.reserve(input.size());

    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::deque<char> currentDeque;
    std::string current;
    for (char c : input) {
        currentDeque.push_back(c);
        // std::string current(currentDeque.begin(), currentDeque.end());
        current = std::string(currentDeque.begin(), currentDeque.end());

        if (dictionary.find(current) == dictionary.end()) {
            currentDeque.pop_back();
            current = std::string(currentDeque.begin(), currentDeque.end());
            lzwEncoded.emplace_back(dictionary[current]);

            if (dictionary.size() < MAX_DICT_SIZE) {
                current += c;
                dictionary[current] = dictionary.size();
            }
            currentDeque.clear();
            currentDeque.push_back(c);
        }
    }

    if (!currentDeque.empty()) {
        std::string current(currentDeque.begin(), currentDeque.end());
        lzwEncoded.emplace_back(dictionary[current]);
    }

    return lzwEncoded;
}

std::vector<char> LZW_Stats::lzwDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    const int MAX_DICT_SIZE = 4096/2;  // Same limit as encoder
    // std::unordered_map<int, std::string> dictionary;
    std::map<int, std::string> dictionary;
    std::vector<char> lzwDecoded;
    lzwDecoded.reserve(input.size());

    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzwDecoded.insert(lzwDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZW decoding error: Invalid code.");
        }

        lzwDecoded.insert(lzwDecoded.end(), entry.begin(), entry.end());

        if (dictionary.size() < MAX_DICT_SIZE) {
            dictionary[dictionary.size()] = current + entry[0];
        }
        current = entry;
    }

    return lzwDecoded;
}


// Functions
void LZW_Stats::Print_Stats() {
    std::cout << "LZW: Average size in bytes: " << avgSizeBytes << "\n";
    std::cout << "LZW: Average encoded time in ms: " << average_time_encoded_in_s  << "\n";
    std::cout << "LZW: Average decoded time in ms: " << average_time_decoded_in_s  << "\n";
    std::cout << "LZW: Average compression ratio: " << average_compression_ratio << "\n";
    std::cout << "LZW: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << "\n";
    std::cout << "LZW: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << "\n";
    std::cout << "LZW: Average encoded throughput: " << average_encoded_throughput << "\n";
    std::cout << "LZW: Average throughput decoded: " << average_decoded_throughput << "\n";
}

void LZW_Stats::Calculate_Cumulative_Average_Stats_For_Directory(int divisor){
    avgSizeBytes /= divisor;
    average_time_encoded_in_s  /= divisor;
    average_time_decoded_in_s  /= divisor;
    average_compression_ratio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    average_encoded_throughput /= divisor;
    average_decoded_throughput /= divisor;
}

void LZW_Stats::getFileStats(std::vector<char> &binaryData, const char* lzwEncodedFileName, const char* lzwDecodedFileName, size_t fileSize, std::filesystem::path& currentDir){
    // floatData = cast<float>(binaryData);
    // Perform lzw encoding
    auto startEncodelzw = std::chrono::high_resolution_clock::now();
    std::vector<int> lzwEncoded = lzwEncode(binaryData);
    auto stopEncodelzw = std::chrono::high_resolution_clock::now();
    auto durationEncodelzw = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncodelzw - startEncodelzw);

    // Perform lzw decoding
    auto startDecodelzw = std::chrono::high_resolution_clock::now();
    std::vector<char> lzwDecoded = lzwDecode(lzwEncoded);
    auto stopDecodelzw = std::chrono::high_resolution_clock::now();
    auto durationDecodelzw = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecodelzw - startDecodelzw);

    // Calculate the peak memory usage
    // avglzwStats.setAvgPeakMemoryDuringDecoding(avglzwStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
    // avglzwStats.setAvgPeakMemoryDuringEncoding(avglzwStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

    // // Calculate the throughput for encoding and decoding
    average_encoded_throughput += static_cast<double>(fileSize) / durationEncodelzw.count() / 1e9; // bytes per second
    average_decoded_throughput += static_cast<double>(fileSize) / durationDecodelzw.count() / 1e9; // bytes per second

    // Verify that no data is lost by comparing decoded data with the original data
    bool lzwDataMatches = binaryData == lzwDecoded;
    assert(lzwDataMatches);
    //std::cout << "lzw Data Matches: " << (lzwDataMatches ? "Yes" : "No") << "\n";

    // Create a binary file from lzw encoded data for further verification
    std::ofstream lzwOutFile(lzwEncodedFileName, std::ios::binary);
    std::ofstream lzwDecodedOutFile(lzwDecodedFileName, std::ios::binary);
    if (!lzwOutFile || !lzwDecodedOutFile) {
        std::cerr << "Error: Unable to create the lzw encoded or decoded file." << "\n";
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

    // open the lzw encoded file and determine the file size
    avgSizeBytes += getFileSize(lzwEncodedFileName);
    //duration cast into a double
    average_time_encoded_in_s  += durationEncodelzw.count() / 1e6;
    average_time_decoded_in_s  += durationDecodelzw.count() / 1e6;
    average_compression_ratio += static_cast<double>(getFileSize(lzwEncodedFileName))/fileSize;

}

 void LZW_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir) {
    std::string lzwEncodedFilename = std::string(filename).erase(std::string(filename).size()-7,6)  + "lzw_encoded_bin";
    std::string lzwDecodedFilename = std::string(filename).erase(std::string(filename).size()-7,6)  + ".lzw_decoded_bin";

     LZW_Stats avgLZWStats;

        for (int i = 0; i < numIterations; ++i) {
        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << "\n";
            std::cout << filename << "\n";
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        // Perform LZW encoding and decoding
        avgLZWStats.getFileStats(binaryData, lzwEncodedFilename.c_str(), lzwDecodedFilename.c_str(), fileSize, currentDir);
    }

    // Calculate the average stats for the current file
    avgLZWStats.Calculate_Cumulative_Average_Stats_For_Directory(numIterations);

    avgSizeBytes += avgLZWStats.getAvgSizeBytes();
    average_time_encoded_in_s  += avgLZWStats.getaverage_time_encoded_in_s ();
    average_time_decoded_in_s  += avgLZWStats.getaverage_time_decoded_in_s ();
    average_compression_ratio += avgLZWStats.getaverage_compression_ratio();
    avgPeakMemoryDuringEncoding += avgLZWStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avgLZWStats.getAvgPeakMemoryDuringDecoding();
    average_encoded_throughput += avgLZWStats.getaverage_encoded_throughput();
    average_decoded_throughput += avgLZWStats.getaverage_decoded_throughput();
 }

// Setters
void LZW_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void LZW_Stats::setaverage_time_encoded_in_s (double value) { average_time_encoded_in_s  = value; }
void LZW_Stats::setaverage_time_decoded_in_s (double value) { average_time_decoded_in_s  = value; }
void LZW_Stats::setaverage_compression_ratio(double value) { average_compression_ratio = value; }
void LZW_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void LZW_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void LZW_Stats::setaverage_encoded_throughput(double value) { average_encoded_throughput = value; }
void LZW_Stats::setaverage_decoded_throughput(double value) { average_decoded_throughput = value; }

// Getters
double LZW_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double LZW_Stats::getaverage_time_encoded_in_s () const { return average_time_encoded_in_s ; }
double LZW_Stats::getaverage_time_decoded_in_s () const { return average_time_decoded_in_s ; }
double LZW_Stats::getaverage_compression_ratio() const { return average_compression_ratio; }
size_t LZW_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t LZW_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double LZW_Stats::getaverage_encoded_throughput() const { return average_encoded_throughput; }
double LZW_Stats::getaverage_decoded_throughput() const { return average_decoded_throughput; }