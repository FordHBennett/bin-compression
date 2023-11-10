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
    average_compressed_file_size = 0.0;
    average_time_encoded_in_microseconds  = 0.0;
    average_time_decoded_in_microseconds  = 0.0;
    average_compression_ratio = 0.0;
    average_encoded_throughput = 0.0;
    average_decoded_throughput = 0.0;
    data_type_byte_size = 0;
}

CommonStats::CommonStats(const CommonStats& other) {
    average_compressed_file_size = other.average_compressed_file_size;
    average_time_encoded_in_microseconds  = other.average_time_encoded_in_microseconds ;
    average_time_decoded_in_microseconds  = other.average_time_decoded_in_microseconds ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

CommonStats& CommonStats::operator=(const CommonStats& other) {
    average_compressed_file_size = other.average_compressed_file_size;
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
    average_compressed_file_size = other.average_compressed_file_size;
    average_time_encoded_in_microseconds  = other.average_time_encoded_in_microseconds ;
    average_time_decoded_in_microseconds  = other.average_time_decoded_in_microseconds ;
    average_compression_ratio = other.average_compression_ratio;
    average_encoded_throughput = other.average_encoded_throughput;
    average_decoded_throughput = other.average_decoded_throughput;
    data_type_byte_size = other.data_type_byte_size;
}

void CommonStats::Reset_Stats() {
    average_compressed_file_size = 0.0;
    average_original_file_size = 0.0;
    average_time_encoded_in_microseconds  = 0.0;
    average_time_decoded_in_microseconds  = 0.0;
    average_compression_ratio = 0.0;
    average_encoded_throughput = 0.0;
    average_decoded_throughput = 0.0;
}

void CommonStats::Write_Stats_To_File(const std::filesystem::path& file_path, const char* compression_type, const std::string& directory_compressed) const {
    // create a json object and write the stats to it
    if(!std::filesystem::exists(file_path.parent_path())){
        std::filesystem::create_directory(file_path.parent_path());
    }
    json stats_json;
    // stats_json["Average File Size"] = average_compressed_file_size;
    stats_json["Average Compression Ratio"] = average_compression_ratio;
    stats_json["Average Encoded Time (microseconds)"] = average_time_encoded_in_microseconds;
    stats_json["Average Decoded Time (microseconds)"] = average_time_decoded_in_microseconds;
    stats_json["Average Encoded Throughput (bytes/microseconds)"] = average_encoded_throughput;
    stats_json["Average Decoded Throughput (bytes/microseconds)"] = average_decoded_throughput;
    stats_json["Data Type Size"] = data_type_byte_size;
    stats_json["Compression Type"] = compression_type;
    stats_json["Directory Compressed"] = directory_compressed;
    stats_json["Average Uncompressed File Size (bytes)"] = average_original_file_size;
    stats_json["Average Compressed File Size (bytes)"] = average_compressed_file_size;

    // write the json object to a file
    std::ofstream stats_file(file_path);
    stats_file << std::setw(4) << stats_json << std::endl;
}

void CommonStats::Calculate_Cumulative_Average_Stats_For_Directory(const int& divisor, const int& number_of_files) {
    average_compressed_file_size /= (divisor*number_of_files);
    // average_time_encoded_in_microseconds  /= static_cast<double>((divisor*number_of_files));
    // average_time_decoded_in_microseconds  /= static_cast<double>((divisor*number_of_files));
    average_compression_ratio /= static_cast<double>((divisor*number_of_files));
    average_encoded_throughput /= static_cast<double>((divisor*number_of_files));
    average_decoded_throughput /= static_cast<double>((divisor*number_of_files));
}

void CommonStats::Is_Little_Endian(){
    //check if the system is little endian
    int num = 1;
    if(*(char *)&num == 1){
        little_endian_flag = true;
    }
    else{
        little_endian_flag = false;
    }
}

void CommonStats::Compute_Compression_Ratio(const std::filesystem::path& original_file_path, const std::filesystem::path& compressed_file_path) {
    average_original_file_size += static_cast<double>(Get_File_Size_Bytes(original_file_path));
    average_compression_ratio += static_cast<double>(Get_File_Size_Bytes(compressed_file_path)) / static_cast<double>(Get_File_Size_Bytes(original_file_path));
}

void CommonStats::Compute_Compressed_File_Size(const std::filesystem::path& file_path) {
    average_compressed_file_size += static_cast<double>(Get_File_Size_Bytes(file_path));
}

void CommonStats::Compute_Encoded_Throughput() {
    average_encoded_throughput = average_original_file_size / average_time_encoded_in_microseconds ;
}

void CommonStats::Compute_Decoded_Throughput() {
    average_decoded_throughput = average_original_file_size / average_time_decoded_in_microseconds ;
}

const bool CommonStats::Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& original_vec, const std::vector<char>& decoded_vec) const {
    return original_vec == decoded_vec;
}

const void CommonStats::Print_Stats(const char* compressionType) const {
    // std::cout << "Average File Size: " << average_compressed_file_size << " bytes" << '\n';
    std::cout << "Average Compression Ratio: " << average_compression_ratio << '\n';
    std::cout << "Average Encoded Time: " << average_time_encoded_in_microseconds  << " microseconds" << '\n';
    std::cout << "Average Decoded Time: " << average_time_decoded_in_microseconds  << " microseconds" << '\n';
    std::cout << "Average Encoded Throughput: " << average_encoded_throughput << " bytes/microseconds" << '\n';
    std::cout << "Average Throughput Decoded: " << average_decoded_throughput << " bytes/microseconds" << '\n';
    // std::cout << "Data size: " << data_type_byte_size << '\n';
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

    for (int side = 0; side < static_cast<int>(Side::NUMBER_SIDES); side++) {
        const std::string side_key = "Side" + std::to_string(side);
#ifdef DEBUG_MODE
        if (!geometa_json.contains(side_key)) {
            ERROR_MSG_AND_EXIT(std::string{"Key '" + side_key + "' not found in JSON file."});
        }
#endif
        const json& side_json = geometa_json[side_key];

        if (!side_json.contains("qT_subsets_resolution")) {
#ifdef DEBUG_MODE
            PRINT_DEBUG(std::string{"NOTE: 'qT_subsets_resolution' key not found in '" + side_key + "'."});
            PRINT_DEBUG(std::string{"NOTE: Make sure that " + side_key + " does not have 'qT_subsets_resolution' "});
#endif

        } else {
            const json resolutions_json = side_json["qT_subsets_resolution"];

#ifdef DEBUG_MODE
            if (resolutions_json.size() > side_resolutions[side].size()) {
                ERROR_MSG_AND_EXIT(std::string{"Resolution array size mismatch for side: " + std::to_string(side)});
            }
#endif

            for (size_t qT_subsets_resolution = 0; qT_subsets_resolution < resolutions_json.size(); qT_subsets_resolution++) {
                side_resolutions[side][qT_subsets_resolution] = resolutions_json[qT_subsets_resolution].get<uint16_t>();
#ifdef DEBUG_MODE
                PRINT_DEBUG(std::string{"Side: " + std::to_string(side) + " Resolution: " + std::to_string(side_resolutions[side][qT_subsets_resolution]) + " at c: " + std::to_string(qT_subsets_resolution)});
#endif
            }
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
