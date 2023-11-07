#include "common_stats.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include "../functions/file_functions.hpp"

#define ERROR_MSG(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << '\n'; \

#define ERROR_MSG_AND_EXIT(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << std::endl; \
    std::exit(EXIT_FAILURE);

#define PRINT_DEBUG(msg) \
    std::cerr << msg << '\n'; \

using json = nlohmann::json;


CommonStats::CommonStats() {
    average_file_size = 0.0;
    average_time_encoded_in_microseconds  = 0.0;
    average_time_decoded_in_microseconds  = 0.0;
    average_compression_ratio = 0.0;
    average_encoded_throughput = 0.0;
    average_decoded_throughput = 0.0;
    data_type_byte_size = 0;
}

CommonStats::CommonStats(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_microseconds  = other.average_time_encoded_in_microseconds ;
    average_time_decoded_in_microseconds  = other.average_time_decoded_in_microseconds ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

CommonStats& CommonStats::operator=(const CommonStats& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_microseconds  = other.average_time_encoded_in_microseconds ;
    average_time_decoded_in_microseconds  = other.average_time_decoded_in_microseconds ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;

    return *this;
}

//move constructor
CommonStats::CommonStats(CommonStats&& other) {
    average_file_size = other.average_file_size;
    average_time_encoded_in_microseconds  = other.average_time_encoded_in_microseconds ;
    average_time_decoded_in_microseconds  = other.average_time_decoded_in_microseconds ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

void CommonStats::Calculate_Cumulative_Average_Stats_For_Directory(const int& divisor, const int& number_of_files) {
    // average_file_size /= (divisor*number_of_files);
    average_time_encoded_in_microseconds  /= static_cast<double>((divisor*number_of_files));
    average_time_decoded_in_microseconds  /= static_cast<double>((divisor*number_of_files));
    average_compression_ratio /= static_cast<double>((divisor*number_of_files));
    // average_encoded_throughput /= static_cast<double>((divisor*number_of_files));
    // average_decoded_throughput /= static_cast<double>((divisor*number_of_files));
}

// template <typename EncodeFunction>
// void CommonStats::Compute_Time_Encoded(EncodeFunction encode) {
//     std::chrono::time_point<std::chrono::high_resolution_clock> start;
//     encode();
//     std::chrono::time_point<std::chrono::high_resolution_clock> end;
//     std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//     average_time_encoded_in_microseconds += duration.count();
// }

// template <typename DecodeFunction>
// void CommonStats::Compute_Time_Decoded(DecodeFunction decode) {
//     std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
//     decode();
//     std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
//     std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//     average_time_encoded_in_microseconds += duration.count();
// }

void CommonStats::Compute_Compression_Ratio(const std::filesystem::path& original_file_path, const std::filesystem::path& compressed_file_path) {
    // PRINT_DEBUG("Original File" + original_file_path.string());
    // PRINT_DEBUG("Original File Size" + std::to_string(Get_File_Size_Bytes(original_file_path)));
    // PRINT_DEBUG("Compressed File" + compressed_file_path.string());
    // PRINT_DEBUG("Compressed File Size" + std::to_string(Get_File_Size_Bytes(compressed_file_path)));
    // PRINT_DEBUG("Compression Ratio: " + std::to_string(Get_File_Size_Bytes(compressed_file_path)) + '/'  + std::to_string(Get_File_Size_Bytes(original_file_path)));
    // PRINT_DEBUG("Compression Ratio: " + std::to_string(static_cast<double>(Get_File_Size_Bytes(compressed_file_path)) / static_cast<double>(Get_File_Size_Bytes(original_file_path))));
    // ERROR_MSG_AND_EXIT("DEBUG");
    average_compression_ratio += static_cast<double>(Get_File_Size_Bytes(compressed_file_path)) / static_cast<double>(Get_File_Size_Bytes(original_file_path));
    // PRINT_DEBUG("Compression Ratio: " + std::to_string(average_compression_ratio));
    
}

void CommonStats::Compute_File_Size(const std::filesystem::path& file_path) {
    average_file_size += static_cast<double>(Get_File_Size_Bytes(file_path));
}

void CommonStats::Compute_Encoded_Throughput() {
    average_encoded_throughput = average_file_size / average_time_encoded_in_microseconds ;
}

void CommonStats::Compute_Decoded_Throughput() {
    average_decoded_throughput = average_file_size / average_time_decoded_in_microseconds ;
}

const bool CommonStats::Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& original_vec, const std::vector<char>& decoded_vec) const {
    return original_vec == decoded_vec;
}

const void CommonStats::Print_Stats(const char* compressionType) const {
    std::cout << "Average File Size: " << average_file_size << " bytes" << '\n';
    std::cout << "Average Compression Ratio: " << average_compression_ratio << '\n';
    std::cout << "Average Encoded Time: " << average_time_encoded_in_microseconds  << " microseconds" << '\n';
    std::cout << "Average Decoded Time: " << average_time_decoded_in_microseconds  << " microseconds" << '\n';
    std::cout << "Average Encoded Throughput: " << average_encoded_throughput << " bytes/microseconds" << '\n';
    std::cout << "Average Throughput Decoded: " << average_decoded_throughput << " bytes/microseconds" << '\n';
    std::cout << "Data Type: " << data_type_byte_size << '\n';
    std::cout << "Compression Type: " << compressionType << '\n';
}


// see if I can avoid using string to reduce the overhead
void CommonStats::Set_Data_Type_Size_And_Side_Resolutions(const std::filesystem::path& geometa_path) {
    auto parse_json = [](const std::filesystem::path& path) -> const json {
        try {
            std::ifstream file(path);
            return json::parse(file, nullptr); // Use non-throwing parse and check for errors afterwards
        } catch( const std::exception& e ) {
            PRINT_DEBUG(std::string{"ERROR: JSON parsing error: "} + std::string{e.what()});
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
#ifdef DEBUG_MODE
    if (data_type_byte_size <= 0) {
        ERROR_MSG_AND_EXIT(std::string{"Invalid 'storage_bytes_size' in " + geometa_path.string()});
    }
#endif

    for (int i = 0; i < static_cast<int>(Side::NUMBER_SIDES); ++i) {
        const std::string side_key = "Side" + std::to_string(i);
#ifdef DEBUG_MODE
        if (!geometa_json.contains(side_key)) {
            ERROR_MSG_AND_EXIT(std::string{"Key '" + side_key + "' not found in JSON file."});
        }
#endif
        const json& side_json = geometa_json[side_key];

#ifdef DEBUG_MODE
        if (!side_json.contains("qT_subsets_resolution")) {
            PRINT_DEBUG(std::string{"NOTE: 'qT_subsets_resolution' key not found in '" + side_key + "'."});
            PRINT_DEBUG(std::string{"NOTE: Make sure that " + side_key + " does not have 'qT_subsets_resolution' "});
            continue;
        }
#endif

        const json resolutions_json = side_json["qT_subsets_resolution"];

#ifdef DEBUG_MODE
        if (resolutions_json.size() > side_resolutions[i].size()) {
            ERROR_MSG_AND_EXIT(std::string{"Resolution array size mismatch for side: " + std::to_string(i)});
        }
#endif

        for (size_t j = 0; j < resolutions_json.size(); ++j) {
            side_resolutions[i][j] = resolutions_json[j].get<uint16_t>();
#ifdef DEBUG_MODE
            PRINT_DEBUG(std::string{"Side: " + std::to_string(i) + " Resolution: " + std::to_string(side_resolutions[i][j]) + " at c: " + std::to_string(j)});
#endif
        }
    }
}

//getters
const int64_t CommonStats::Get_Side_Resolution(const uint8_t& lod_number) const {
    return (1 + (1 << (6 + lod_number)));
}
const int CommonStats::Get_Data_Type_Size() const {
    return data_type_byte_size;
}
