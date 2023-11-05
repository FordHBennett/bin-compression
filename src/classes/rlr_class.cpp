#include "rlr_class.hpp"
#include <fstream>
#include <iostream>
#include <vector>
//Constructors
RLR::RLR(){}

RLR::RLR(const RLR& other) {}

RLR& RLR::operator=(const RLR& other) {}

RLR::~RLR() {}

void RLR::encode(const std::vector<char>& input) {
    // std::vector<char> encoded;
    // this->encodedData = encoded;
}

// void RLR::decode(const std::vector<char>& input) {
    // std::vector<char> decoded;
    // for(int byte_index = 0; byte_index<input.size();byte_index+=this->Get_Data_Type_Size()){
    //     for(int8_t i = 0; i< static_cast<int8_t>(input.at(byte_index)); i++){
    //         for(int j = 1; j < this->Get_Data_Type_Size(); j++){
    //             decoded.emplace_back(input.at(byte_index+j));
    //         }
    //     }
    // }

    // this->decodedData = decoded;
// }

const void RLR::writeEncodedFile(const std::vector<char>& encodedData, const char* filename) {
    std::ofstream encodedOutFile(filename, std::ios::binary);
    if (!encodedOutFile) {
        std::cout << "Error: Unable to create the run-length encoded file." << '\n';
    }

    // Writing encoded data to file
    encodedOutFile.write(encodedData.data(), encodedData.size());

    encodedOutFile.close();
}

const void RLR::writeDecodedFile(const std::vector<char>& decodedData, const char* filename) {
    std::ofstream decodedOutFile(filename, std::ios::binary);
    if (!decodedOutFile) {
        std::cout << "Error: Unable to create the run-length decoded file." << '\n';
    }

    // Writing decoded data to file
    decodedOutFile.write(decodedData.data(), decodedData.size());

    decodedOutFile.close();
}


//change std::pair<char, uint8_t>  to std::pair<T, uint8_t>
// Shift rlr encoding by 1 so that 0 run length represents 1 run length
// which allows us to have 256 and stuff
// create a virtural function
// std::vector<std::pair<char, uint8_t>> RLR::encodeRow(const std::vector<char>& input, int row, int data_type_size) {
//     std::vector<std::pair<char, uint8_t>> encodedRow;

//     // Ensure the segment doesn't go beyond the input's bounds
//     size_t end = std::min(static_cast<size_t>(row + data_type_size), static_cast<size_t>(input.size()));

//     if (row >= end) {
//         return encodedRow; // Return empty result for empty input segment
//     }

//     char currentChar = input[row];
//     uint8_t runLength = 1;

//     for (size_t i = row + 1; i < end; ++i) {
//         if (input[i] == currentChar && runLength < 255) {
//             runLength++;
//         } else {
//             encodedRow.emplace_back(currentChar, runLength);
//             currentChar = input[i];
//             runLength = 1;
//         }
//     }

//     encodedRow.emplace_back(currentChar, runLength);  // Add the last run for this row.
//     return encodedRow;
// }

// std::vector<std::pair<char, uint8_t>> RLR::encode(const std::vector<char>& input, int data_type_size) {
//     if (input.empty() || data_type_byte_size <= 0) {
//         return {};
//     }

//     std::vector<std::pair<char, uint8_t>> encoded;
//     for (size_t i = 0; i < input.size(); i += data_type_size) {
//         auto rowResult = encodeRow(input, i, data_type_size);
//         encoded.insert(encoded.end(), rowResult.begin(), rowResult.end());
//     }

//     return encoded;
// }

// std::vector<char> RLR::decodeRow(const std::vector<std::pair<char, uint8_t>>& input, int row, int data_type_size) {
//     std::vector<char> decodedRow;

//     // Calculate the starting position and ending position based on row and data_type_size
//     int start = row;
//     int end = std::min(start + data_type_size, static_cast<int>(input.size()));

//     for (int i = start; i < end; ++i) {
//         decodedRow.insert(decodedRow.end(), input[i].second, input[i].first);
//     }

//     return decodedRow;
// }


// std::vector<char> RLR::decode(const std::vector<std::pair<char, uint8_t>>& input, int data_type_size) {
//     std::vector<char> decoded;

//     // Estimate the total length and reserve capacity.
//     size_t totalLength = 0;
//     for (const auto& pair : input) {
//         totalLength += pair.second;
//     }
//     decoded.reserve(totalLength);

//     for (size_t i = 0; i < input.size(); i += data_type_size) {
//         auto rowResult = decodeRow(input, i, data_type_size);
//         decoded.insert(decoded.end(), rowResult.begin(), rowResult.end());
//     }

//     return decoded;
// }




// void RLR::getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* dencodedFilename, size_t fileSize, std::filesystem::path& currentDir){
//     //find the .geometa file
//     std::filesystem::path geometaFile;
//     for (const auto& entry : std::filesystem::recursive_directory_iterator(currentDir)) {
//         if (entry.is_regular_file() && entry.path().extension() == ".geometa") {
//             geometaFile = entry.path();
//             break; // Optional: stop after finding the first .geometa file
//         }
//     }
//     //read the .geometa file which is a json file
//     // Step 2: Read the .geometa file which is a json file
//     std::ifstream inFile(geometaFile);
//     if (!inFile.is_open()) {
//         std::cout << "Failed to open the .geometa file." << '\n';
//     }

//     std::string line;
//     std::string datatype;
//     while (std::getline(inFile, line)) {
//         // Assuming the line format is "key": "value"
//         size_t keyStart = line.find("\"store_sc_type\":");
//         if (keyStart != std::string::npos) {
//             size_t valueStart = line.find('"', keyStart + 16); // 16 = length of "store_sc_type":
//             if (valueStart != std::string::npos) {
//                 size_t valueEnd = line.find('"', valueStart + 1);
//                 if (valueEnd != std::string::npos) {
//                     datatype = line.substr(valueStart + 1, valueEnd - valueStart - 1);
//                     //std::cout << datatype << '\n';
//                     break; // Optional: stop after finding the first match
//                 }
//             }
//         }
//     }

//     inFile.close();

//     int datatype_size = 0;
//     //this will be a case statement
//     if (datatype == "int16" || datatype == "uint16") {
//         datatype_size = 16;
//     }


//     // Perform run-length encoding
//     auto startEncod = std::chrono::high_resolution_clock::now();
//     auto encoded = encode(binaryData, datatype_size);
//     auto stopEncode = std::chrono::high_resolution_clock::now();
//     auto durationEncode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncode - startEncod);

//     // Perform run-length decoding
//     auto startDecode = std::chrono::high_resolution_clock::now();
//     auto decoded = decode(encoded, datatype_size);
//     auto stopDecode = std::chrono::high_resolution_clock::now();
//     auto durationDecode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecode - startDecode);

//     // Calculate the peak memory usage
//     // avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
//     // avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

//     // Verify that no data is lost by comparing decoded data with the original data
//     bool dataMatches = binaryData == decoded;
//     assert(dataMatches);
//     //std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << '\n';

//     // Write the encoded and decoded data to files
//     writeEncodedFile(encoded, encodedFilename);
//     writeDecodedFile(decoded, dencodedFilename);


//     // open the run-length encoded file and determine the file size
//     avgSizeBytes += getFileSize(encodedFilename);
//     average_time_encoded_in_ns  += durationEncode.count() * 1000000;
//     average_time_encoded_in_ns  += durationDecode.count() * 1000000;
//     average_compression_ratio += static_cast<double>(getFileSize(encodedFilename))/fileSize;
//     average_encoded_throughput += static_cast<double>(fileSize) / durationEncode.count() * 1000000000; // bytes per second
//     average_decoded_throughput += static_cast<double>(fileSize) / durationDecode.count() * 1000000000; // bytes per second

// }


// Control_Stats RLR::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir, CommonStats &localStats) {



//     // Calculate the average stats for the current file
//     localStats.Calculate_Cumulative_Average_Stats_For_Directory(numIterations);

//     return localStats;


// }

//getters
const char* RLR::getCompressionType() const {return compressionType;}

const std::vector<char> RLR::getEncodedData() const {return encodedData;}

const std::vector<char> RLR::getDecodedData() const {return decodedData;}