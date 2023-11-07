#include "common_stats.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <sstream> 
#include <iomanip>
#include "../functions/file_functions.hpp"
#include "../functions/debug_functions.hpp"

using json = nlohmann::json;


CommonStats::CommonStats() {
    average_file_size = 0.0;
    average_time_encoded_in_ns  = 0.0;
    average_time_encoded_in_ns  = 0.0;
    average_compression_ratio = 0.0;
    average_encoded_throughput = 0.0;
    average_decoded_throughput = 0.0;
    data_type_byte_size = 0;
}

CommonStats::CommonStats(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

CommonStats& CommonStats::operator=(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;

    return *this;
}

//move constructor
CommonStats::CommonStats(CommonStats&& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_time_encoded_in_ns  = other.average_time_encoded_in_ns ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

void CommonStats::Calculate_Cumulative_Average_Stats_For_Directory(const int& divisors, const int& files) {
    average_file_size /= (divisors*files);
    average_time_encoded_in_ns  /= (divisors*files);
    average_time_encoded_in_ns  /= (divisors*files);
    average_compression_ratio /= (divisors*files);
    average_encoded_throughput /= (divisors*files);
    average_decoded_throughput /= (divisors*files);
}

// void CommonStats::Compute_Time_Encoded(const std::function<void(std::vector<char>)>& encode, const std::vector<char>& data) {
//     auto start = std::chrono::high_resolution_clock::now();
//     encode(data);
//     auto end = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
//     average_time_encoded_in_ns  += duration.count();
// }

// void CommonStats::Compute_Time_Decoded(const std::function<void(std::vector<char>)>& decode, const std::vector<char>& data) {
//     auto start = std::chrono::high_resolution_clock::now();
//     decode(data);
//     auto end = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
//     average_time_encoded_in_ns  += duration.count();
// }

template <typename EncodeFunction>
void CommonStats::Compute_Time_Encoded(EncodeFunction encode, const std::vector<char>& data) {

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    encode(data);
    std::chrono::time_point<std::chrono::high_resolution_clock> end;
    std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    average_time_encoded_in_ns += duration.count();
}

template <typename DecodeFunction>
void CommonStats::Compute_Time_Decoded(DecodeFunction decode, const std::vector<char>& data) {
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    decode(data);
    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    average_time_encoded_in_ns += duration.count();
}

void CommonStats::Compute_Compression_Ratio(const std::filesystem::path& original_file_path, const std::filesystem::path& compressed_file_path) {
    average_compression_ratio += Get_File_Size_Bytes(compressed_file_path) / Get_File_Size_Bytes(original_file_path);
}

void CommonStats::Compute_File_Size(const std::filesystem::path& file_path) {
    average_file_size += Get_File_Size_Bytes(file_path);
}

void CommonStats::Compute_Encoded_Throughput() {
    average_encoded_throughput = average_file_size / average_time_encoded_in_ns ;
}

void CommonStats::Compute_Decoded_Throughput() {
    average_decoded_throughput = average_file_size / average_time_encoded_in_ns ;
}

const bool CommonStats::Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& original_vec, const std::vector<char>& decoded_vec) const {
    return original_vec == decoded_vec;
}

const void CommonStats::Print_Stats(const char* compressionType) const {
    std::cout << "Average File Size: " << average_file_size << " bytes" << '\n';
    std::cout << "Average Encoded Time: " << average_time_encoded_in_ns  << " seconds" << '\n';
    std::cout << "Average Decoded Time: " << average_time_encoded_in_ns  << " seconds" << '\n';
    std::cout << "Average Compression Ratio: " << average_compression_ratio << '\n';
    std::cout << "Average Encoded Throughput: " << average_encoded_throughput << " bytes/seconds" << '\n';
    std::cout << "Average Throughput Decoded: " << average_decoded_throughput << " bytes/seconds" << '\n';
    std::cout << "Data Type: " << data_type_byte_size << '\n';
    std::cout << "Compression Type: " << compressionType << '\n';
}


// see if I can avoid using string to reduce the overhead
void CommonStats::Set_Data_Type_Size_And_Side_Resolutions(const std::filesystem::path& geometa_path) {
    // std::ifstream geometa_file(geometa_path);

    // if (!geometa_file.is_open()) {
    //     std::cerr << "ERROR: Could not open " << geometa_path.c_str() << '\n';
    //     std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << '\n';
    //     ERROR_MSG_AND_EXIT("ERROR");
    // }

    // json geometa_json;
    // try {
    //     geometa_file >> geometa_json;
    //     std::cout << "Check read data: " << std::setw(4) << geometa_json << std::endl;
    // } catch (const json::parse_error& e) {
    //     std::cerr << "ERROR: JSON parsing error: " << e.what() << '\n';
    //     std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << '\n';
    //             ERROR_MSG_AND_EXIT("ERROR");

    // }

    // //list all the keys in the json file
    // // for (auto& el : geometa_json.items()) {
    // //     std::cout << el.key() << " : " << el.value() << '\n';
    // // }
    // // std::exit(1);

    // // Check if the key 'storage_bytes_size' exists.
    // if (geometa_json.find("storage_bytes_size") == geometa_json.end()) {
    //     std::cerr << "ERROR: 'storage_bytes_size' key not found in JSON file." << '\n';
    //     std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << '\n';
    //     ERROR_MSG("ERROR");
    //     exit(EXIT_FAILURE);
    // }

    // data_type_byte_size = geometa_json["storage_bytes_size"].get<int8_t>();
    // if (data_type_byte_size == 0) {
    //     std::cerr << "ERROR: Invalid or missing 'storage_bytes_size' in " << geometa_path.c_str() << '\n';
    //     std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path() << '\n';
    //     ERROR_MSG("ERROR");
    //     exit(EXIT_FAILURE);
    // }

    // try {
    //     // std::cout << geometa_json.dump() << '\n';
    //     for(int i = 0; i < static_cast<int>(Side::NUMBER_SIDES); i++) {
    //         std::string side_key = "Side" + std::to_string(i);
    //         if (geometa_json.find(side_key) == geometa_json.end()) {
    //             std::cerr << "ERROR: Key '" << side_key << "' not found in JSON file." << '\n';
    //             ERROR_MSG("ERROR");
    //         }

    //         // Check if 'qT_subsets_resolution' key exists
    //         if(!geometa_json[side_key].contains("qT_subsets_resolution")) {
    //             std::cerr << "NOTE: 'qT_subsets_resolution' key not found in '" << side_key << "'." << '\n';
    //             std::cerr << "NOTE: Make sure that " << side_key << " does not have 'qT_subsets_resolution' " << '\n';
    //             const auto& resolutions_json = geometa_json[side_key]["qT_subsets_resolution"];
    //             for (size_t j = 0; j < resolutions_json.size(); ++j) {
    //                 side_resolutions[i][j] = 0;
    //             }
    //         } else {
    //             const auto& resolutions_json = geometa_json[side_key]["qT_subsets_resolution"];

    //             // Store the resolutions
    //             for (size_t j = 0; j < resolutions_json.size(); ++j) {
    //                 side_resolutions[i][j] = resolutions_json[j].get<uint16_t>();
    //             }

    //             // Print the resolutions
    //             for (size_t j = 0; j < resolutions_json.size(); ++j) {
    //                 std::cerr << "Side: " << i << " Resolution: " << side_resolutions[i][j] << " at c: " << std::to_string(j) << '\n';
    //             }
    //         }
    //     }
    // } catch (const json::exception& e) {
    //     std::cerr << "ERROR: JSON error: " << e.what() << '\n';
    //     ERROR_MSG_AND_EXIT("ERROR");
    // }

    // geometa_file.close();

    // std::ifstream geometa_file(geometa_path, std::ios::in);
    // if (!geometa_file) {
    //     ERROR_MSG_AND_EXIT("Could not open file: " + geometa_path.string());
    // }
    auto parse_json = [](const std::filesystem::path& path) -> const json {
        try {
            std::ifstream file(path);
            return json::parse(file, nullptr); // Use non-throwing parse and check for errors afterwards
        } catch( const std::exception& e ) {
            // std::cerr << "ERROR: JSON parsing error: " << e.what() << '\n';
            PRINT_DEBUG(std::string{"ERROR: JSON parsing error: "} + std::string{e.what()});
            // std::cerr << "ERROR: You are in directory: " << std::filesystem::current_path().c_str() << '\n';
            PRINT_DEBUG(std::string{"ERROR: You are in directory: "} + std::filesystem::current_path().string());
            ERROR_MSG_AND_EXIT("ERROR");
        }
    };

    const json geometa_json = parse_json(geometa_path);

#ifdef DEBUG_MODE
    // std::cout << "Check read data: " << std::setw(4) << geometa_json << std::endl;
    std::ostringstream oss;
    oss << "Check read data: " << std::setw(4) << geometa_json;
    PRINT_DEBUG(std::string{"Check read data: "} + oss.str());
#endif

    // Use 'at' for access to throw json::out_of_range if the key does not exist
    try {
        data_type_byte_size = geometa_json.at("storage_bytes_size").get<int8_t>();
    } catch (const json::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"'storage_bytes_size' key not found or invalid in JSON file."});
    }

    if (data_type_byte_size <= 0) {
        ERROR_MSG_AND_EXIT(std::string{"Invalid 'storage_bytes_size' in " + geometa_path.string()});
    }

    for (int i = 0; i < static_cast<int>(Side::NUMBER_SIDES); ++i) {
        const std::string side_key = "Side" + std::to_string(i);
        if (!geometa_json.contains(side_key)) {
            ERROR_MSG_AND_EXIT(std::string{"Key '" + side_key + "' not found in JSON file."});
        }

        const json& side_json = geometa_json[side_key];

        if (!side_json.contains("qT_subsets_resolution")) {
            // std::cerr << "NOTE: 'qT_subsets_resolution' key not found in '" << side_key << "'." << '\n';
            // std::cerr << "NOTE: Make sure that " << side_key << " does not have 'qT_subsets_resolution' " << '\n';
            PRINT_DEBUG(std::string{"NOTE: 'qT_subsets_resolution' key not found in '" + side_key + "'."});
            PRINT_DEBUG(std::string{"NOTE: Make sure that " + side_key + " does not have 'qT_subsets_resolution' "});
            continue; // Or handle the error as needed
        }

        const json resolutions_json = side_json["qT_subsets_resolution"];
        if (resolutions_json.size() > side_resolutions[i].size()) {
            ERROR_MSG_AND_EXIT(std::string{"Resolution array size mismatch for side: " + std::to_string(i)});
        }

        for (size_t j = 0; j < resolutions_json.size(); ++j) {
            side_resolutions[i][j] = resolutions_json[j].get<uint16_t>();
            // std::cerr << "Side: " << i << " Resolution: " << side_resolutions[i][j] << " at c: " << j << '\n';
            PRINT_DEBUG(std::string{"Side: " + std::to_string(i) + " Resolution: " + std::to_string(side_resolutions[i][j]) + " at c: " + std::to_string(j)});
        }
    }
}

//getters
const int64_t CommonStats::Get_Side_Resolution(const uint8_t& lod_number) const {
    return (1 + std::pow(2,6+lod_number));
    // return (0x1 +(2 << (6 + lod_number)));
}
const int CommonStats::Get_Data_Type_Size() const {
    return data_type_byte_size;
}
