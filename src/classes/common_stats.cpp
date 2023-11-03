#include "common_stats.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include "../functions/file_functions.h"

#define ERROR_MSG(msg) \
    std::cerr << "Error: " << msg \
              << " (File: " << __FILE__ \
              << ", Line: " << __LINE__ << ")" << "\n"; \
    std::exit(EXIT_FAILURE);


using json = nlohmann::json;


CommonStats::CommonStats() {
    average_file_size = 0.0;
    average_time_encoded_in_s  = 0.0;
    average_time_decoded_in_s  = 0.0;
    average_compression_ratio = 0.0;
    average_encoded_throughput = 0.0;
    average_decoded_throughput = 0.0;
    data_type_byte_size = 0;
}

CommonStats::CommonStats(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_s  = other.average_time_encoded_in_s ;
    average_time_decoded_in_s  = other.average_time_decoded_in_s ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

CommonStats& CommonStats::operator=(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_s  = other.average_time_encoded_in_s ;
    average_time_decoded_in_s  = other.average_time_decoded_in_s ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;

    return *this;
}

CommonStats::~CommonStats() {}

void CommonStats::Calculate_Cumulative_Average_Stats_For_Directory(int divisors, int files) {
    average_file_size /= (divisors*files);
    average_time_encoded_in_s  /= (divisors*files);
    average_time_decoded_in_s  /= (divisors*files);
    average_compression_ratio /= (divisors*files);
    average_encoded_throughput /= (divisors*files);
    average_decoded_throughput /= (divisors*files);
}

void CommonStats::Compute_Time_Encoded(std::function<void(std::vector<char>)> encode, std::vector<char> data) {
    auto start = std::chrono::high_resolution_clock::now();
    encode(data);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
    average_time_encoded_in_s  += duration.count();
}

void CommonStats::Compute_Time_Decoded(std::function<void(std::vector<char>)> decode, std::vector<char> data) {
    auto start = std::chrono::high_resolution_clock::now();
    decode(data);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
    average_time_decoded_in_s  += duration.count();
}

void CommonStats::Compute_Compression_Ratio(const std::filesystem::path original_file_path, const std::filesystem::path compressed_file_path) {
    average_compression_ratio += Get_File_Size_Bytes(compressed_file_path) / Get_File_Size_Bytes(original_file_path);
}

void CommonStats::Compute_File_Size(const std::filesystem::path file_path) {
    average_file_size += Get_File_Size_Bytes(file_path);
}

// void CommonStats::Compute_Encoded_Throughput() {
//     average_encoded_throughput = average_file_size / average_time_encoded_in_s ;
// }

// void CommonStats::Compute_Decoded_Throughput() {
//     average_decoded_throughput = average_file_size / average_time_decoded_in_s ;
// }

// const bool CommonStats::Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& originalData, const std::vector<char>& decodedData) const {
//     return originalData == decodedData;
// }

const void CommonStats::Print_Stats(const char* compressionType) const {
    std::cout << "Average File Size: " << average_file_size << " bytes" << "\n";
    std::cout << "Average Encoded Time: " << average_time_encoded_in_s  << " seconds" << "\n";
    std::cout << "Average Decoded Time: " << average_time_decoded_in_s  << " seconds" << "\n";
    std::cout << "Average Compression Ratio: " << average_compression_ratio << "\n";
    std::cout << "Average Encoded Throughput: " << average_encoded_throughput << " bytes/seconds" << "\n";
    std::cout << "Average Throughput Decoded: " << average_decoded_throughput << " bytes/seconds" << "\n";
    std::cout << "Data Type: " << data_type_byte_size << "\n";
    std::cout << "Compression Type: " << compressionType << "\n";
}

void CommonStats::Set_Data_Type_Size_And_Side_Resolutions(const std::filesystem::path geometa_path) {
    std::ifstream geometa_file(geometa_path);

    if (!geometa_file.is_open()) {
        std::cerr << "ERROR: Could not open " << geometa_path.c_str() << "\n";
        std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << "\n";
        ERROR_MSG("ERROR");
        exit(EXIT_FAILURE);
    }

    json geometa_json;
    try {
        geometa_file >> geometa_json;
        std::cout << "Check read data: " << std::setw(4) << geometa_json << std::endl;
    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: JSON parsing error: " << e.what() << "\n";
        std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << "\n";
        ERROR_MSG("ERROR");
        exit(EXIT_FAILURE);
    }

    //list all the keys in the json file
    // for (auto& el : geometa_json.items()) {
    //     std::cout << el.key() << " : " << el.value() << "\n";
    // }
    // std::exit(1);

    // Check if the key 'storage_bytes_size' exists.
    if (geometa_json.find("storage_bytes_size") == geometa_json.end()) {
        std::cerr << "ERROR: 'storage_bytes_size' key not found in JSON file." << "\n";
        std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << "\n";
        ERROR_MSG("ERROR");
        exit(EXIT_FAILURE);
    }

    data_type_byte_size = geometa_json["storage_bytes_size"].get<int8_t>();
    if (data_type_byte_size == 0) {
        std::cerr << "ERROR: Invalid or missing 'storage_bytes_size' in " << geometa_path.c_str() << "\n";
        std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path() << "\n";
        ERROR_MSG("ERROR");
        exit(EXIT_FAILURE);
    }

    try {
        // for (int i = 0; i < static_cast<int>(Side::NUMBER_SIDES); i++) {
        //     std::string side_key = "Side" + std::to_string(i);
        //     if (geometa_json.find(side_key) == geometa_json.end()) {
        //         std::cerr << "ERROR: Key '" << side_key << "' not found in JSON file." << "\n";
        //         std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path() << "\n";
        //         ERROR_MSG("ERROR");
        //     }

        //     // auto& resolutions = side_resolutions[static_cast<size_t>(i)];
        //     auto resolutions;
        //     for(uint j = 0; j < geometa_json[side_key]["qT_subsets_resolution"].size(); j++) {
        //         uint16_t resolution = geometa_json[side_key]["qT_subsets_resolution"][j].get<uint16_t>();
        //         resolutions[j] = resolution;
        //     }

        //     side_resolutions[static_cast<size_t>(i)] = resolutions;

        //     // print the resolutions along with the index

        //     for (uint j = 0; j < resolutions.size(); j++) {
        //         std::cout << "Side: " << i << " Resolution: " << resolutions[j] << " at c: " << std::to_string(j) << "\n";
        //     }

        //     //print the resolutions from json file
        //     for (auto& el : geometa_json[side_key]["qT_subsets_resolution"].items()) {
        //         std::cout << " Side: " << i << "  Resolution: " << el.value() << "\n";
        //     }

        // }
        // std::cout << geometa_json.dump() << "\n";
        for(int i = 0; i < static_cast<int>(Side::NUMBER_SIDES); i++) {
            std::string side_key = "Side" + std::to_string(i);
            if (geometa_json.find(side_key) == geometa_json.end()) {
                std::cerr << "ERROR: Key '" << side_key << "' not found in JSON file." << "\n";
                ERROR_MSG("ERROR");
            }

            // Check if 'qT_subsets_resolution' key exists
            if(!geometa_json[side_key].contains("qT_subsets_resolution")) {
                std::cerr << "NOTE: 'qT_subsets_resolution' key not found in '" << side_key << "'." << "\n";
                std::cerr << "NOTE: Make sure that " << side_key << " does not have 'qT_subsets_resolution' " << "\n";
                const auto& resolutions_json = geometa_json[side_key]["qT_subsets_resolution"];
                for (size_t j = 0; j < resolutions_json.size(); ++j) {
                    side_resolutions[i][j] = 0;
                }
            } else {
                const auto& resolutions_json = geometa_json[side_key]["qT_subsets_resolution"];

                // Store the resolutions
                for (size_t j = 0; j < resolutions_json.size(); ++j) {
                    side_resolutions[i][j] = resolutions_json[j].get<uint16_t>();
                }

                // Print the resolutions
                for (size_t j = 0; j < resolutions_json.size(); ++j) {
                    std::cerr << "Side: " << i << " Resolution: " << side_resolutions[i][j] << " at c: " << std::to_string(j) << "\n";
                }
            }

        }
    } catch (const json::exception& e) {
        std::cerr << "ERROR: JSON error: " << e.what() << "\n";
        ERROR_MSG("ERROR");
        exit(EXIT_FAILURE);
    }

    geometa_file.close();
}
