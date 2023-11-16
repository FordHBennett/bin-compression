#include "shannon_fano.hpp"
// #include "../functions/file_functions.hpp"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp> // Include the JSON library

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

ShannonFano::ShannonFano() {

}

void ShannonFano::Write_Frequencies_To_JSON_File(const std::filesystem::path& binary_file_path, const std::filesystem::path& json_file_path) const {
    size_t data_type_size = this->Get_Data_Type_Size();
    std::unordered_map<std::string, size_t> frequency_map;

    PRINT_DEBUG("File to be processed: " + binary_file_path.string());
    // Open the binary file for reading
    std::ifstream binary_file(binary_file_path, std::ios::binary);
    if (!binary_file.is_open()) {
        ERROR_MSG_AND_EXIT("Unable to open binary file: " + binary_file_path.string());
    }

    std::vector<char> buffer(data_type_size);
    while (binary_file.read(buffer.data(), data_type_size)) {
        std::string data_unit(buffer.data(), data_type_size);
        frequency_map[data_unit]++;
    }
    binary_file.close();

    // sort the map by value in descending order
    std::vector<std::pair<std::string, size_t>> frequency_vec(frequency_map.begin(), frequency_map.end());
    // std::sort(frequency_vec.begin(), frequency_vec.end(), [](const std::pair<std::string, size_t>& a, const std::pair<std::string, size_t>& b) {
    //     return a.second > b.second;
    // });

    // Create a JSON object
    nlohmann::json json_freq;
    for (const auto& [data_unit, frequency] : frequency_vec) {
        json_freq[data_unit] = frequency;
    }

    // Write the JSON object to a file
    // std::ofstream json_file(json_file_path);
    // if (!json_file.is_open()) {
    //     ERROR_MSG_AND_EXIT("Unable to open JSON file: " + json_file_path.string());
    // }
    // check if the file exists
    std::ofstream json_file;
    if (std::filesystem::exists(json_file_path)) {
        // json_file.open(json_file_path, std::ios_base::app);
        ERROR_MSG_AND_EXIT("JSON file already exists: " + json_file_path.string());
    }
    json_file.open(json_file_path);


    json_file << json_freq.dump(4);
    // add the binary file path to the JSON file
    json_file << "\n\n" << "Binary file path: " << binary_file_path.string();
    json_file.close();
}