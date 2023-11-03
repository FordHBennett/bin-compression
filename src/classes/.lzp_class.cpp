#include "lzp_class.h"

//Constructors
LZP_Stats::LZP_Stats() :
    avgSizeBytes(0.0), average_time_encoded_in_s (0.0), average_time_decoded_in_s (0.0),
    average_compression_ratio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    average_encoded_throughput(0.0), average_decoded_throughput(0.0) {}

LZP_Stats::LZP_Stats(double avgSizeBytes, double average_time_encoded_in_s , double average_time_decoded_in_s , double average_compression_ratio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double average_encoded_throughput, double average_decoded_throughput) :
    avgSizeBytes(avgSizeBytes), average_time_encoded_in_s (average_time_encoded_in_s ), average_time_decoded_in_s (average_time_decoded_in_s ),
    average_compression_ratio(average_compression_ratio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    average_encoded_throughput(average_encoded_throughput), average_decoded_throughput(average_decoded_throughput) {}

// Lempel-Ziv Parallel (LZP) encoding
// Time Complexity:
// Best, Average:O(nlogn) due to the sorting of rotations.
// Worst:O(n^2 logn) because of the lexicographical comparison in the worst case.
// Space Complexity:
// Best, Average, Worst: O(n)

std::vector<int> LZP_Stats::lzpEncode(const std::vector<char>& input) {

    const int MAX_DICT_SIZE = 4096;
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzpEncoded;
    lzpEncoded.reserve(input.size());

    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::deque<char> currentDeque;
    for (char c : input) {
        currentDeque.push_back(c);
        std::string current(currentDeque.begin(), currentDeque.end());

        if (dictionary.find(current) == dictionary.end()) {
            currentDeque.pop_back();
            current = std::string(currentDeque.begin(), currentDeque.end());
            lzpEncoded.emplace_back(dictionary[current]);

            if (dictionary.size() < MAX_DICT_SIZE) {
                dictionary[current + c] = dictionary.size();
            }
            currentDeque.clear();
            currentDeque.push_back(c);
        }
    }

    if (!currentDeque.empty()) {
        std::string current(currentDeque.begin(), currentDeque.end());
        lzpEncoded.emplace_back(dictionary[current]);
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

    const int MAX_DICT_SIZE = 4096;
    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzpDecoded;
    lzpDecoded.reserve(input.size());

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

        if (dictionary.size() < MAX_DICT_SIZE) {
            dictionary[dictionary.size()] = current + entry[0];
        }
        current = entry;
    }

    return lzpDecoded;
}



// Functions
void LZP_Stats::Print_Stats() {
    std::cout << "LZP: Average size in bytes: " << avgSizeBytes << "\n";
    std::cout << "LZP: Average encoded time in ms: " << average_time_encoded_in_s  << "\n";
    std::cout << "LZP: Average decoded time in ms: " << average_time_decoded_in_s  << "\n";
    std::cout << "LZP: Average compression ratio: " << average_compression_ratio << "\n";
    std::cout << "LZP: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << "\n";
    std::cout << "LZP: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << "\n";
    std::cout << "LZP: Average encoded throughput: " << average_encoded_throughput << "\n";
    std::cout << "LZP: Average throughput decoded: " << average_decoded_throughput << "\n";
}

void LZP_Stats::Calculate_Cumulative_Average_Stats_For_Directory(int divisor){
    avgSizeBytes /= divisor;
    average_time_encoded_in_s  /= divisor;
    average_time_decoded_in_s  /= divisor;
    average_compression_ratio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    average_encoded_throughput /= divisor;
    average_decoded_throughput /= divisor;
}

void LZP_Stats::getFileStats(std::vector<char> &binaryData, const char* lzpEncodedFileName, const char* lzpDecodedFileName, size_t fileSize, std::filesystem::path& currentDir){

    // Perform LZP encoding
    auto startEncodeLzp = std::chrono::high_resolution_clock::now();
    std::vector<int> lzpEncoded = lzpEncode(binaryData);
    auto stopEncodeLzp = std::chrono::high_resolution_clock::now();
    auto durationEncodeLzp = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncodeLzp - startEncodeLzp);

    // Perform LZP decoding
    auto startDecodeLzp = std::chrono::high_resolution_clock::now();
    std::vector<char> lzpDecoded = lzpDecode(lzpEncoded);
    auto stopDecodeLzp = std::chrono::high_resolution_clock::now();
    auto durationDecodeLzp = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecodeLzp - startDecodeLzp);

    // Calculate the peak memory usage
    // avgLZPStats.setAvgPeakMemoryDuringDecoding(avgLZPStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
    // avgLZPStats.setAvgPeakMemoryDuringEncoding(avgLZPStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

    // Calculate the throughput for encoding and decoding


    // Verify that no data is lost by comparing decoded data with the original data
    bool dataMatches = binaryData == lzpDecoded;
    assert(dataMatches);
    //std::cout << "LZP Data Matches: " << (dataMatches ? "Yes" : "No") << "\n";

    // Create a binary file from LZP encoded data for further verification
    std::ofstream lzpOutFile(lzpEncodedFileName, std::ios::binary);
    std::ofstream lzpDecodedOutFile(lzpDecodedFileName, std::ios::binary);
    if (!lzpOutFile || !lzpDecodedOutFile) {
        std::cerr << "Error: Unable to create the LZP encoded or decoded file." << "\n";
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
    average_time_encoded_in_s  += durationEncodeLzp.count() * 1000000;
    average_time_decoded_in_s  += durationDecodeLzp.count() * 1000000;
    average_compression_ratio += static_cast<double>(getFileSize(lzpEncodedFileName))/fileSize;
    average_encoded_throughput += binaryData.size() / static_cast<double>(durationEncodeLzp.count()) * 1000000000; // bytes/sec
    average_decoded_throughput += lzpDecoded.size() / static_cast<double>(durationDecodeLzp.count()) * 1000000000; // bytes/sec
}

 void LZP_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir) {
    //std::cout << "Compressing " << filename << " using LZP" << "\n";
    std::string lzpEncodedFilename = std::string(filename) + ".lzp.bin";
    std::string lzpDecodedFilename = std::string(filename) + ".lzp_decoded.bin";

     LZP_Stats avglzpStats;

        for (int i = 0; i < numIterations; ++i) {
        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << "\n";
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        // Perform lzp encoding and decoding
        avglzpStats.getFileStats(binaryData, lzpEncodedFilename.c_str(), lzpDecodedFilename.c_str(), fileSize, currentDir);
    }

    // Calculate the average stats for the current file
    avglzpStats.Calculate_Cumulative_Average_Stats_For_Directory(numIterations);

    avgSizeBytes += avglzpStats.getAvgSizeBytes();
    average_time_encoded_in_s  += avglzpStats.getaverage_time_encoded_in_s ();
    average_time_decoded_in_s  += avglzpStats.getaverage_time_decoded_in_s ();
    average_compression_ratio += avglzpStats.getaverage_compression_ratio();
    avgPeakMemoryDuringEncoding += avglzpStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avglzpStats.getAvgPeakMemoryDuringDecoding();
    average_encoded_throughput += avglzpStats.getaverage_encoded_throughput();
    average_decoded_throughput += avglzpStats.getaverage_decoded_throughput();
 }

// Setters
void LZP_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void LZP_Stats::setaverage_time_encoded_in_s (double value) { average_time_encoded_in_s  = value; }
void LZP_Stats::setaverage_time_decoded_in_s (double value) { average_time_decoded_in_s  = value; }
void LZP_Stats::setaverage_compression_ratio(double value) { average_compression_ratio = value; }
void LZP_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void LZP_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void LZP_Stats::setaverage_encoded_throughput(double value) { average_encoded_throughput = value; }
void LZP_Stats::setaverage_decoded_throughput(double value) { average_decoded_throughput = value; }

// Getters
double LZP_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double LZP_Stats::getaverage_time_encoded_in_s () const { return average_time_encoded_in_s ; }
double LZP_Stats::getaverage_time_decoded_in_s () const { return average_time_decoded_in_s ; }
double LZP_Stats::getaverage_compression_ratio() const { return average_compression_ratio; }
size_t LZP_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t LZP_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double LZP_Stats::getaverage_encoded_throughput() const { return average_encoded_throughput; }
double LZP_Stats::getaverage_decoded_throughput() const { return average_decoded_throughput; }