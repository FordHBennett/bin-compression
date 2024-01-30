#include "shannon_fano.hpp"
// #include "../functions/file_functions.hpp"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
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

void ShannonFano::Write_Geobin_Data_As_Header_To_File(const std::filesystem::path& binary_path, const std::filesystem::path& header_path, const int& row_number, const int& number_of_bytes_to_read) const {
    std::vector<char> binary_data_vec;
    std::ifstream input_file(binary_path, std::ios::binary);
#ifdef DEBUG
    if(!input_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }
#endif

    binary_data_vec.resize(number_of_bytes_to_read);
    // populate the binary data vector with the data from the file starting at number_of_bytes_to_read * row
    std::streampos start_position = static_cast<std::streampos>(number_of_bytes_to_read) * row_number;
    input_file.seekg(start_position);
#ifdef DEBUG
    if (input_file.fail()) {
        ERROR_MSG_AND_EXIT("Error: Unable to seek to the specified position in the file.");
    }
#endif

    // Read the data from the file into the vector.
    input_file.read(reinterpret_cast<char*>(binary_data_vec.data()), number_of_bytes_to_read);
#ifdef DEBUG
if (input_file.fail() && !input_file.eof()) {
    ERROR_MSG_AND_EXIT("Error: Unable to read from the file.");
}
#endif
    input_file.close();

    // std::ofstream output_file(header_path, std::ios::binary);
    // append to the file
    std::ofstream output_file(header_path, std::ios::binary | std::ios::app);
#ifdef DEBUG
    if(!output_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }
#endif
    // PRINT_DEBUG("Writing to file: " << header_path);
    if(row_number == 0) {
        // write #pragma once to the file
        output_file << "#pragma once" << "\n\n";
    }

    // remove all - from the path and replace them with _
    std::string binary_path_string = binary_path.string();
    std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
    // remove all / from the path and replace them with _
    std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
    // remove all . from the path and replace them with _
    std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');

    output_file <<  "#define " << binary_path_string << "_row_" << row_number << " = {";
    for(int i = 0; i < binary_data_vec.size(); i++) {
        // Cast each byte to unsigned int before outputting
        output_file << static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i]));
        if(i != binary_data_vec.size() - 1) {
            output_file << ", ";
        }
        if(i % 10 == 0 && i != 0) {
            // output_file << "\\" << '\n;
            //write \ and newline to the file
            output_file << "\\\n";
        }
    }
    output_file << "}\n\n";
    output_file.close();
}

void ShannonFano::Write_Binary_Frequencies_Per_Row_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& row_length, const uint64_t file_size) const{
    std::vector<char> binary_data_vec;
    const uint8_t volatile data_type_size = this->Get_Data_Type_Size();
    std::ifstream input_file(binary_path, std::ios::binary);

#ifdef DEBUG
    if(!input_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }
#endif

    // previous row variables
    std::unordered_map<char, int> previous_byte_to_index_map;
    std::vector<std::pair<char, int>> previous_byte_frequencies_vec;
    size_t previous_row_number = 0;
    size_t previous_start_position = 0;

    for(size_t row_number = 0; row_number < file_size / row_length; row_number++){
        binary_data_vec.resize(row_length*2);
        // populate the binary data vector with the data from the file starting at row_length * row
        std::streampos start_position = static_cast<std::streampos>(row_length) * row_number;
        input_file.seekg(start_position, std::ios::beg);

    // #ifdef DEBUG
        if (input_file.fail()) {
            std::cerr << "Error: Unable to seek to the specified position in the file." << '\n';
            std::cerr << "position: " << start_position << '\n';
            std::cerr << "row_length: " << row_length << '\n';
            std::cerr << "row_number: " << row_number << '\n';
            std::cerr << "file_size: " << file_size << '\n';
            std::cerr << "binary_path: " << binary_path << '\n';
            std::cerr << "json_path: " << json_path << '\n';
            std::cerr << "current_path: " << std::filesystem::current_path() << '\n';
            ERROR_MSG_AND_EXIT("Error: Unable to seek to the specified position in the file.");
        }
    // #endif

        // Read the data from the file into the vector.
        input_file.read(reinterpret_cast<char*>(binary_data_vec.data()), row_length);
        // if(binary_data_vec.size() == 0){
        //     ERROR_MSG_AND_EXIT("Error: Unable to read from the file.");
        // }
    // #ifdef DEBUG
    if (input_file.fail() && !input_file.eof()) {
        ERROR_MSG_AND_EXIT("Error: Unable to read from the file.");
    }
    // #endif
        // input_file.close();

        switch(data_type_size){
            case 1:{
                //get the frequencies of each byte in the binary data vector
                std::unordered_map<char, int> byte_frequencies;
                for(int i = 0; i < binary_data_vec.size(); i++){
                    byte_frequencies[binary_data_vec[i]]++;
                }
                // std::ofstream output_file(json_path, std::ios::binary);
                // append to the file
                std::ofstream output_file(json_path, std::ios::binary | std::ios::app);
    #ifdef DEBUG
        if(!output_file) {
            ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
        }
    #endif

                std::string binary_path_string = binary_path.string();
                std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
                std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
                std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');

                //sort the map by value
                std::vector<std::pair<char, int>> byte_frequencies_vec(byte_frequencies.begin(), byte_frequencies.end());
                std::sort(byte_frequencies_vec.begin(), byte_frequencies_vec.end(), [](const std::pair<char, int>& a, const std::pair<char, int>& b){
                    return a.second > b.second;
                });

                // map the the most frequent byte to  0, the second most frequent byte to 1, etc. to create a lookup table
                std::unordered_map<char, int> byte_to_index_map;
                for(int i = 0; i < byte_frequencies_vec.size(); i++){
                    byte_to_index_map[byte_frequencies_vec[i].first] = i;
                }

                start_position = output_file.tellp();
                // write the json to the file
                // if((byte_to_index_map == previous_byte_to_index_map) && row_number != 0 && (row_number % 100 == 0)){
                //     // delete bytes from previous_start_position to end of file
                //     output_file.close();
                //     output_file.open(json_path, std::ios::binary | std::ios::out | std::ios::in);
                //     output_file.seekp(previous_start_position);

                //     output_file << "{\n";
                //     output_file << "    \"" << binary_path_string << "_row_" << previous_row_number << '_' << row_number << "\": {\n";
                //     output_file << "        \"total_amount_of_unique_bytes\": " << previous_byte_frequencies_vec.size() << ",\n";
                //     // write the lookup table to the file
                //     output_file << "        \"data_type_size\": 1" << ",\n";
                //     output_file << "        \"lookup_table\": {\n";
                //     for(auto it = previous_byte_to_index_map.begin(); it != previous_byte_to_index_map.end(); it++){
                //         output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                //         if(std::next(it) != previous_byte_to_index_map.end()){
                //             output_file << ",\n";
                //         } else {
                //             output_file << "\n";
                //         }
                //     }
                //     output_file << "        }\n";
                //     output_file << "    }\n";
                //     output_file << "}\n\n";
                // }else{
                //     previous_row_number = row_number;
                //     previous_byte_to_index_map = byte_to_index_map;
                //     previous_byte_frequencies_vec = byte_frequencies_vec;
                //     previous_start_position = start_position;
                // }
                if(byte_frequencies_vec.size() < 16){
                    output_file << "{\n";
                    output_file << "    \"" << binary_path_string << "_row_" << row_number << "\": {\n";
                    output_file << "        \"total_amount_of_unique_bytes\": " << byte_frequencies_vec.size() << ",\n";
                    // write the lookup table to the file
                    output_file << "        \"data_type_size\": " << data_type_size << ",\n";
                    output_file << "        \"lookup_table\": {\n";
                    for(auto it = byte_to_index_map.begin(); it != byte_to_index_map.end(); it++){
                        output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                        if(std::next(it) != byte_to_index_map.end()){
                            output_file << ",\n";
                        } else {
                            output_file << "\n";
                        }
                    }
                    output_file << "        }\n";
                    output_file << "    }\n";
                    output_file << "}\n\n";
                }


                output_file.close();
                break;
            }
            case 2:{
                //get the frequencies of each 2 byte pair in the binary data vector
                std::unordered_map<std::string, int> byte_pair_frequencies;
                for(int i = 0; i < binary_data_vec.size(); i+=2){
                    std::string byte_pair = std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i]))) + std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+1])));
                    byte_pair_frequencies[byte_pair]++;
                }
                // std::ofstream output_file(json_path, std::ios::binary);
                // append to the file
                std::ofstream output_file(json_path, std::ios::binary | std::ios::app);
    #ifdef DEBUG
        if(!output_file) {
            ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
        }
    #endif

                // remove all - from the path and replace them with _
                std::string binary_path_string = binary_path.string();
                std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
                // remove all / from the path and replace them with _
                std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
                // remove all . from the path and replace them with _
                std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');

                // sort the map by value
                std::vector<std::pair<std::string, int>> byte_pair_frequencies_vec(byte_pair_frequencies.begin(), byte_pair_frequencies.end());
                std::sort(byte_pair_frequencies_vec.begin(), byte_pair_frequencies_vec.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
                    return a.second > b.second;
                });

                // map the the most frequent 2 byte pair to {0,0}, the second most frequent 2 pair byte to {0,1}, etc. to create a lookup table
                std::unordered_map<std::string, int> byte_pair_to_index_map;
                int first_counter = 0;
                int second_counter = 0;

                for (int i = 0; i < byte_pair_frequencies_vec.size(); i++) {
                    uint16_t count = first_counter << 8 | second_counter;
                    byte_pair_to_index_map[byte_pair_frequencies_vec[i].first] = count;
                    second_counter++;
                    if (second_counter == UINT8_MAX) { // Assuming a range of 0-255 for each byte
                        second_counter = 0;
                        first_counter++;
                    }
                }


                // write the lookup table to the file
                if(byte_pair_frequencies_vec.size() < UINT8_MAX){
                    output_file << "{\n";
                    output_file << "    \"" << binary_path_string << "_row_" << row_number << "\": {\n";
                    output_file << "        \"total_amount_of_unique_bytes\": " << byte_pair_frequencies_vec.size() << ",\n";
                    output_file << "        \"data_type_size\": " << data_type_size << ",\n";
                    output_file << "        \"lookup_table\": {\n";
                    for (auto it = byte_pair_to_index_map.begin(); it != byte_pair_to_index_map.end(); it++) {
                        // output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first[0])) << static_cast<unsigned int>(static_cast<uint8_t>(it->first[1])) << "\": [" << it->second[0] << ", " << it->second[1] << "]";
                        output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first[0])) << static_cast<unsigned int>(static_cast<uint8_t>(it->first[1])) << "\": " << it->second;
                        if (std::next(it) != byte_pair_to_index_map.end()) {
                            output_file << ",\n";
                        } else {
                            output_file << "\n";
                        }
                    }

                    output_file << "        }\n";
                    output_file << "    }\n";
                    output_file << "}\n\n";
                }
                output_file.close();

            }
            case 4:{
                //get the frequencies of each 4 byte pair in the binary data vector
                std::unordered_map<std::string, int> byte_pair_frequencies;
                for(int i = 0; i < binary_data_vec.size(); i+=4){
                    std::string byte_pair = std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i]))) + std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+1]))) + std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+2]))) + std::to_string(static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+3])));
                    byte_pair_frequencies[byte_pair]++;
                }
                // std::ofstream output_file(json_path, std::ios::binary);
                // append to the file
                std::ofstream output_file(json_path, std::ios::binary | std::ios::app);
    #ifdef DEBUG
        if(!output_file) {
            ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
        }
    #endif

                // remove all - from the path and replace them with _
                std::string binary_path_string = binary_path.string();
                std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
                // remove all / from the path and replace them with _
                std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
                // remove all . from the path and replace them with _
                std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');

                // sort the map by value
                std::vector<std::pair<std::string, int>> byte_pair_frequencies_vec(byte_pair_frequencies.begin(), byte_pair_frequencies.end());
                std::sort(byte_pair_frequencies_vec.begin(), byte_pair_frequencies_vec.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
                    return a.second > b.second;
                });
                // map the the most frequent 4 byte pair to {0,0,0,0}, the second most frequent 4 pair byte to {0,0,0,1}, etc. to create a lookup table
                std::unordered_map<std::string, uint32_t> byte_pair_to_index_map;
                int first_counter = 0;
                int second_counter = 0;
                int third_counter = 0;
                int fourth_counter = 0;
                for (int i = 0; i < byte_pair_frequencies_vec.size(); i++) {
                    uint32_t count = first_counter << 24 | second_counter << 16 | third_counter << 8 | fourth_counter;
                    byte_pair_to_index_map[byte_pair_frequencies_vec[i].first] = count;
                    fourth_counter++;
                    if (fourth_counter == 256) { // Assuming a range of 0-255 for each byte
                        fourth_counter = 0;
                        third_counter++;
                    }
                    if (third_counter == 256) { // Assuming a range of 0-255 for each byte
                        third_counter = 0;
                        second_counter++;
                    }
                    if (second_counter == 256) { // Assuming a range of 0-255 for each byte
                        second_counter = 0;
                        first_counter++;
                    }
                }

                // write the json to the file similar to case 2 but with 4 bytes instead of 2
                if(byte_pair_frequencies_vec.size() < UINT16_MAX){
                    output_file << "{\n";
                    output_file << "    \"" << binary_path_string << "_row_" << row_number << "\": {\n";
                    output_file << "        \"total_amount_of_unique_bytes\": " << byte_pair_frequencies_vec.size() << ",\n";
                    output_file << "        \"lookup_table\": {\n";
                    for (auto it = byte_pair_to_index_map.begin(); it != byte_pair_to_index_map.end(); it++) {
                        output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first[0])) << static_cast<unsigned int>(static_cast<uint8_t>(it->first[1])) << static_cast<unsigned int>(static_cast<uint8_t>(it->first[2])) << static_cast<unsigned int>(static_cast<uint8_t>(it->first[3])) << "\": " << it->second;
                        if (std::next(it) != byte_pair_to_index_map.end()) {
                            output_file << ",\n";
                        } else {
                            output_file << "\n";
                        }
                    }
                    output_file << "        }\n";
                    output_file << "    }\n";
                    output_file << "}\n\n";
                }
                output_file.close();
                break;

            }
            default:
                ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
                break;
        }
    }
    //if output file is empty then delete it
    std::filesystem::path output_file_path = json_path;
    std::ifstream output_file(output_file_path);
    if(output_file.peek() == std::ifstream::traits_type::eof()){
        output_file.close();
        std::filesystem::remove(output_file_path);
    }


}

std::string ShannonFano::Format_Path_String(const std::filesystem::path& path) const {
    std::string formatted_string = path.string();
    std::replace(formatted_string.begin(), formatted_string.end(), '-', '_');
    std::replace(formatted_string.begin(), formatted_string.end(), '/', '_');
    std::replace(formatted_string.begin(), formatted_string.end(), '.', '_');
    return formatted_string;
}

std::string ShannonFano::Format_Byte_Pair(int byte_pair) const {
    std::stringstream ss;
    ss << "[" << ((byte_pair >> 8) & 0xFF) << ", " << (byte_pair & 0xFF) << "]";
    return ss.str();
}

std::string ShannonFano::Format_Byte_Quartet(uint32_t byte_quartet) const {
    std::stringstream ss;
    ss << "[" << ((byte_quartet >> 24) & 0xFF) << ", "
       << ((byte_quartet >> 16) & 0xFF) << ", "
       << ((byte_quartet >> 8) & 0xFF) << ", "
       << (byte_quartet & 0xFF) << "]";
    return ss.str();
}

// Definitions for helper functions
std::ifstream ShannonFano::Open_File(const std::filesystem::path& path, std::ios::openmode mode) const {
    std::ifstream file(path, mode);
    if (!file) {
        throw std::runtime_error("Error: Unable to open file at " + path.string());
    }
    return file;
}

std::vector<char> ShannonFano::Read_Binary_Data(std::ifstream& input_file, int number_of_bytes) const {
    std::vector<char> data(number_of_bytes);
    input_file.read(reinterpret_cast<char*>(data.data()), number_of_bytes);
    if (input_file.fail() && !input_file.eof()) {
        throw std::runtime_error("Error: Unable to read from the binary file.");
    }
    return data;
}

template <typename T>
std::pair<std::unordered_map<T, int>, std::vector<std::pair<T, int>>> ShannonFano::Create_And_Sort_Frequency_Vector(const std::vector<char>& data) const {
    std::unordered_map<T, int> frequencies;
    for (auto byte : data) {
        frequencies[byte]++;
    }
    std::vector<std::pair<T, int>> freq_vec(frequencies.begin(), frequencies.end());
    std::sort(freq_vec.begin(), freq_vec.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    return {frequencies, freq_vec};
}

std::string ShannonFano::Generate_Macro_Name(const std::filesystem::path& path) const {
    std::string name = path.stem().string() + "_LUT";
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    return name;
}

template<typename T>
std::unordered_map<T, int> ShannonFano::Create_Lookup_Table(const std::vector<std::pair<T, int>>& frequency_vec) {
    std::unordered_map<T, int> lookup_table;
    for (int i = 0; i < frequency_vec.size(); ++i) {
        lookup_table[frequency_vec[i].first] = i;
    }
    return lookup_table;
}

template<typename T>
void ShannonFano::Write_To_Json(std::ofstream& output_file, const std::filesystem::path& binary_path, uint8_t data_type_size, const std::vector<std::pair<T, int>>& frequency_vec, const std::unordered_map<T, int>& lookup_table) {
    output_file << "{\n";
    output_file << "    \"" << Format_Path_String(binary_path) << "\": {\n";
    output_file << "        \"Data Type Size\": " << static_cast<int>(data_type_size) << ",\n";
    output_file << "        \"Lookup Table\": {\n";

    for (auto it = frequency_vec.begin(); it != frequency_vec.end(); ++it) {
        T byte_value = it->first;
        int index = lookup_table.at(byte_value);
        output_file << "            \"" << Format_Byte_Value(byte_value) << "\": {\n";
        output_file << "                \"Index\": " << index << ",\n";
        output_file << "                \"Frequency\": " << it->second << "\n";
        output_file << "            }" << (std::next(it) != frequency_vec.end() ? "," : "") << "\n";
    }

    output_file << "        }\n";
    output_file << "    }\n";
    output_file << "}\n";
}


std::string ShannonFano::Format_Byte_Value(char byte) const {
    return std::to_string(static_cast<int>(byte));
}

std::string ShannonFano::Format_Byte_Value(int byte_pair) const {
    std::stringstream ss;
    ss << "[" << ((byte_pair >> 8) & 0xFF) << ", " << (byte_pair & 0xFF) << "]";
    return ss.str();
}


std::string ShannonFano::Format_Byte_Value(uint32_t byte_quartet) const {
    std::stringstream ss;
    ss << "[" << ((byte_quartet >> 24) & 0xFF) << ", "
       << ((byte_quartet >> 16) & 0xFF) << ", "
       << ((byte_quartet >> 8) & 0xFF) << ", "
       << (byte_quartet & 0xFF) << "]";
    return ss.str();
}




void ShannonFano::Write_Binary_Frequencies_Per_File_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& number_of_bytes_to_read) {
    auto input_file = Open_File(binary_path, std::ios::binary);
    auto binary_data_vec = Read_Binary_Data(input_file, number_of_bytes_to_read);

    std::ofstream output_file(json_path, std::ios::binary | std::ios::trunc);
    if (!output_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the JSON file.");
    }

    const uint8_t data_type_size = this->Get_Data_Type_Size();

    switch(data_type_size) {
        case 1: {
            auto [byte_frequencies, byte_frequencies_vec] = Create_And_Sort_Frequency_Vector<u_char>(binary_data_vec);
            auto lookup_table = Create_Lookup_Table<u_char>(byte_frequencies_vec);
            Write_To_Json<u_char>(output_file, binary_path, data_type_size, byte_frequencies_vec, lookup_table);

            // Write the lookup table to the JSON file
            // if (lookup_table.size() < 16) {
                // Write_To_Json(output_file, binary_path, data_type_size, byte_frequencies_vec, lookup_table);
            // }
            break;
        }
        case 2: {
            auto [byte_pair_frequencies, byte_pair_frequencies_vec] = Create_And_Sort_Frequency_Vector<int>(binary_data_vec);
            auto lookup_table = Create_Lookup_Table<int>(byte_pair_frequencies_vec);
            // Write_To_Json(output_file, binary_path, data_type_size, byte_pair_frequencies_vec, lookup_table);
            Write_To_Json<int>(output_file, binary_path, data_type_size, byte_pair_frequencies_vec, lookup_table);

            // Write the lookup table to the JSON file
            // if (lookup_table.size() < UINT8_MAX) {
                // Write_To_Json(output_file, binary_path, data_type_size, byte_pair_frequencies_vec, lookup_table);
            // }
            break;
        }
        case 4: {
            auto [byte_quartet_frequencies, byte_quartet_frequencies_vec] = Create_And_Sort_Frequency_Vector<uint32_t>(binary_data_vec);
            auto lookup_table = Create_Lookup_Table<uint32_t>(byte_quartet_frequencies_vec);
            // Write_To_Json(output_file, binary_path, data_type_size, byte_quartet_frequencies_vec, lookup_table);
            Write_To_Json<uint32_t>(output_file, binary_path, data_type_size, byte_quartet_frequencies_vec, lookup_table);
            // if (lookup_table.size() < UINT16_MAX) {
                // Write_To_Json(output_file, binary_path, data_type_size, byte_quartet_frequencies_vec, lookup_table);
            // }
            break;
        }
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }

    // If output file is empty then delete it
    if(output_file.tellp() == 0) {
        output_file.close();
        std::filesystem::remove(json_path);
    }

    output_file.close();

}

void ShannonFano::Write_Lookup_Table_To_Header_File(const std::filesystem::path& binary_path, const std::filesystem::path& header_path, const int& number_of_bytes_to_read) const {
    auto input_file = Open_File(binary_path, std::ios::binary);
    auto binary_data_vec = Read_Binary_Data(input_file, number_of_bytes_to_read);

    std::ofstream output_file(header_path, std::ios::binary | std::ios::app);
    if (!output_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the header file.");
    }

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    std::string macro_name = Generate_Macro_Name(binary_path);

    switch(data_type_size) {
        case 1: {
            auto [byte_frequencies, byte_frequencies_vec] = Create_And_Sort_Frequency_Vector<char>(binary_data_vec);
            std::unordered_map<char, int> lookup_table;
            for (int i = 0; i < byte_frequencies_vec.size(); ++i) {
                lookup_table[byte_frequencies_vec[i].first] = i;
            }

            if (lookup_table.size() < 16) {
                std::string macro_name = Format_Path_String(binary_path) + "_LUT";
                std::transform(macro_name.begin(), macro_name.end(), macro_name.begin(), ::toupper); // Convert macro name to uppercase
                output_file << "#ifndef " << macro_name << "_H\n";
                output_file << "#define " << macro_name << "_H\n";
                output_file << "namespace PlanetData_LUT {\n";
                output_file << "\n";
                output_file << "#define " << macro_name << " { \\\n";
                for (const auto& pair : byte_frequencies_vec) {
                    char byte = pair.first;
                    int index = lookup_table[byte];
                    if(index > 15){
                        ERROR_MSG_AND_EXIT("Error: Index is greater than 15.");
                    }

                    output_file << "    { " << static_cast<int>(byte) << ", " << index << " }";
                    if (pair != byte_frequencies_vec.back()) {
                        output_file << ", \\\n";
                    }
                }
                output_file << "}\n";
                output_file << "}\n";  // End of namespace
                output_file << "#endif // " << macro_name << "_H\n";
            }

            break;
        }
        case 2: {
            auto [byte_pair_frequencies, byte_pair_frequencies_vec] = Create_And_Sort_Frequency_Vector<int>(binary_data_vec);

            // Create lookup table mapping byte pairs to indices
            std::unordered_map<int, int> lookup_table;
            for (int i = 0; i < byte_pair_frequencies_vec.size(); ++i) {
                lookup_table[byte_pair_frequencies_vec[i].first] = i;
            }

            if (lookup_table.size() < UINT8_MAX) {
                std::string macro_name = Format_Path_String(binary_path) + "_LUT";
                std::transform(macro_name.begin(), macro_name.end(), macro_name.begin(), ::toupper); // Convert macro name to uppercase

                output_file << "#ifndef " << macro_name << "_H\n";
                output_file << "#define " << macro_name << "_H\n";
                output_file << "namespace PlanetData_LUT {\n";
                output_file << "\n";
                output_file << "#define " << macro_name << " { \\\n";
                for (const auto& pair : byte_pair_frequencies_vec) {
                    int byte_pair = pair.first;
                    int index = lookup_table[byte_pair];
                    if(index > 255){
                        ERROR_MSG_AND_EXIT("Error: Index is greater than 255.");
                    }
                    output_file << "    { "  << "{ " << static_cast<int>((byte_pair >> 8 & 0xFF)) << ", " << static_cast<int>((byte_pair) & 0xFF) << " }" << ", " << index << " }";
                    if (pair != byte_pair_frequencies_vec.back()) {
                        output_file << ", \\\n";
                    }
                }

                output_file << "}\n";
                output_file << "}\n";  // End of namespace
                output_file << "#endif // " << macro_name << "_H\n";
            }
            break;
        }
        case 4: {
            auto [byte_quartet_frequencies, byte_quartet_frequencies_vec] = Create_And_Sort_Frequency_Vector<uint32_t>(binary_data_vec);

            std::unordered_map<uint32_t, int> lookup_table;
            for (int i = 0; i < byte_quartet_frequencies_vec.size(); ++i) {
                lookup_table[byte_quartet_frequencies_vec[i].first] = i;
            }
            std::string macro_name = Format_Path_String(binary_path) + "_LUT";
            std::transform(macro_name.begin(), macro_name.end(), macro_name.begin(), ::toupper); // Convert macro name to uppercase

            if (lookup_table.size() < UINT16_MAX) {
                output_file << "#ifndef " << macro_name << "_H\n";
                output_file << "#define " << macro_name << "_H\n";
                output_file << "namespace PlanetData_LUT {\n";
                output_file << "\n";
                output_file << "#define " << macro_name << " { \\\n";
                for (const auto& pair : byte_quartet_frequencies_vec) {
                    uint32_t byte_quartet = pair.first;
                    int index = lookup_table[byte_quartet];
                    if(index > 65535){
                        ERROR_MSG_AND_EXIT("Error: Index is greater than 65535.");
                    }
                    if(lookup_table.size() < UINT8_MAX){
                        output_file << "    { {"
                                    << static_cast<int>((byte_quartet >> 24) & 0xFF) << ", "
                                    << static_cast<int>((byte_quartet >> 16) & 0xFF) << ", "
                                    << static_cast<int>((byte_quartet >> 8) & 0xFF) << ", "
                                    << static_cast<int>(byte_quartet & 0xFF)
                                    << " },  "
                                    << index
                                    << " }";
                    } else{
                            output_file << "    {  {"
                                        << static_cast<int>((byte_quartet >> 24) & 0xFF) << ", "
                                        << static_cast<int>((byte_quartet >> 16) & 0xFF) << ", "
                                        << static_cast<int>((byte_quartet >> 8) & 0xFF) << ", "
                                        << static_cast<int>(byte_quartet & 0xFF)
                                        << " }, { "
                                        << static_cast<int>((index >> 8) & 0xFF) << ", "
                                        << static_cast<int>(index & 0xFF)
                                        << " } }";
                    }
                    if (pair != byte_quartet_frequencies_vec.back()) {
                        output_file << ", \\\n";
                    }
                }

                output_file << "}\n";
                output_file << "}\n";  // End of namespace
                output_file << "#endif // " << macro_name << "_H\n";
            }
            break;
        }
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }

    if(output_file.tellp() == 0) {
        std::filesystem::remove(header_path);
    }
    output_file.close();
}