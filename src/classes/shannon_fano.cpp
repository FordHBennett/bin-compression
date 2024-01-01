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
    int previous_row_number = 0;
    size_t previous_start_position = 0;

    for(int row_number = 0; row_number < file_size / row_length; row_number++){
        binary_data_vec.resize(row_length);
        // populate the binary data vector with the data from the file starting at row_length * row
        std::streampos start_position = static_cast<std::streampos>(row_length) * row_number;
        input_file.seekg(start_position);
    #ifdef DEBUG
        if (input_file.fail()) {
            ERROR_MSG_AND_EXIT("Error: Unable to seek to the specified position in the file.");
        }
    #endif

        // Read the data from the file into the vector.
        input_file.read(reinterpret_cast<char*>(binary_data_vec.data()), row_length);
    #ifdef DEBUG
    if (input_file.fail() && !input_file.eof()) {
        ERROR_MSG_AND_EXIT("Error: Unable to read from the file.");
    }
    #endif
        input_file.close();

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
                if((byte_frequencies_vec == previous_byte_frequencies_vec) && row_number != 0 && (row_number % 100 == 0)){
                    // delete bytes from start_position to end of file
                    output_file.close();
                    output_file.open(json_path, std::ios::binary | std::ios::out | std::ios::in);
                    output_file.seekp(start_position);

                    output_file << "{\n";
                    output_file << "    \"" << binary_path_string << "_row_" << previous_row_number << '_' << row_number << "\": {\n";
                    output_file << "        \"total_amount_of_unique_bytes\": " << previous_byte_frequencies_vec.size() << ",\n";
                    // write the lookup table to the file
                    output_file << "        \"data_type_size\": 1" << ",\n";
                    output_file << "        \"lookup_table\": {\n";
                    for(auto it = previous_byte_to_index_map.begin(); it != previous_byte_to_index_map.end(); it++){
                        output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                        if(std::next(it) != previous_byte_to_index_map.end()){
                            output_file << ",\n";
                        } else {
                            output_file << "\n";
                        }
                    }
                    output_file << "        }\n";
                    output_file << "    }\n";
                    output_file << "}\n\n";
                    break;
                }
                // if(byte_frequencies_vec.size() < 16){
                //     output_file << "{\n";
                //     output_file << "    \"" << binary_path_string << "_row_" << row_number << "\": {\n";
                //     output_file << "        \"total_amount_of_unique_bytes\": " << byte_frequencies_vec.size() << ",\n";
                //     // write the lookup table to the file
                //     output_file << "        \"data_type_size\": " << data_type_size << ",\n";
                //     output_file << "        \"lookup_table\": {\n";
                //     for(auto it = byte_to_index_map.begin(); it != byte_to_index_map.end(); it++){
                //         output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                //         if(std::next(it) != byte_to_index_map.end()){
                //             output_file << ",\n";
                //         } else {
                //             output_file << "\n";
                //         }
                //     }
                //     output_file << "        }\n";
                //     output_file << "    }\n";
                //     output_file << "}\n\n";
                // }
                previous_row_number = row_number;
                previous_byte_to_index_map = byte_to_index_map;
                previous_byte_frequencies_vec = byte_frequencies_vec;
                previous_start_position = start_position;

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

void ShannonFano::Write_Binary_Frequencies_Per_File_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& number_of_bytes_to_read) const{
    std::vector<char> binary_data_vec;
    const uint8_t volatile data_type_size = this->Get_Data_Type_Size();
    std::ifstream input_file(binary_path, std::ios::binary);
#ifdef DEBUG
    if(!input_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }
#endif

    binary_data_vec.resize(number_of_bytes_to_read);
    // populate the binary data vector with the data from the file starting at number_of_bytes_to_read * row
    std::streampos start_position = 0;
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

            // remove all - from the path and replace them with _
            std::string binary_path_string = binary_path.string();
            std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
            // remove all / from the path and replace them with _
            std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
            // remove all . from the path and replace them with _
            std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');

            //sort the map by value
            std::vector<std::pair<char, int>> byte_frequencies_vec(byte_frequencies.begin(), byte_frequencies.end());
            std::sort(byte_frequencies_vec.begin(), byte_frequencies_vec.end(), [](const std::pair<char, int>& a, const std::pair<char, int>& b){
                return a.second > b.second;
            });

            // write the json to the file
            if(byte_frequencies_vec.size() < 16){
                output_file << "{\n";
                // output_file << "    \"" << binary_path_string << "\": {\n";
                // output_file << "        \"total_amount_of_unique_bytes\": " << byte_frequencies_vec.size() << ",\n";
                // write the lookup table to the file
                output_file << "        " << binary_path_string << ": {\n";
                // output_file << "        \"lookup_table\": {\n";
                // output_file << "#pragma once\n\n";

                // output_file << "const uint8_t " << binary_path_string << "_lookup_table[" << std::to_string(byte_frequencies_vec.size()) << "] = {";

                // for(auto it = byte_frequencies_vec.begin(); it != byte_frequencies_vec.end(); it++){
                //     output_file << static_cast<unsigned int>(static_cast<uint8_t>(it->first));
                //     if(std::next(it) != byte_frequencies_vec.end()){
                //         output_file << ", ";
                //     }
                // }

                // iterate through the sorted vector and write the bytes to the file
                // uint64_t count = 0;
                for (auto it = byte_frequencies_vec.begin(); it != byte_frequencies_vec.end(); it++) {
                    output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                    if (std::next(it) != byte_frequencies_vec.end()) {
                        output_file << ",\n";
                    } else {
                        output_file << "\n";
                    }
                    // count++;
                }
                // output_file << "        }\n";
                output_file << "    }\n";
                output_file << "}\n\n";
            }
            break;
        }
        case 2:{
            // use int, int where the key is the first and second byte of the char array and the value is the frequency
            std::unordered_map<int, int> byte_pair_frequencies;
            for(int i = 0; i < binary_data_vec.size(); i+=2){
                int byte_pair = static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i])) << 8 | static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+1]));
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
            // binary_path_string = binary_path_string + std::string{"_lookup_table"};

            // sort the map by value where value = 0 is the most frequent byte pair
            std::vector<std::pair<int, int>> byte_pair_frequencies_vec(byte_pair_frequencies.begin(), byte_pair_frequencies.end());
            std::sort(byte_pair_frequencies_vec.begin(), byte_pair_frequencies_vec.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b){
                return a.second > b.second;
            });

            // change the value of the map to be the index of the byte pair in the sorted vector
            std::unordered_map<int, int> byte_pair_to_index_map;
            for(int i = 0; i < byte_pair_frequencies_vec.size(); i++){
                byte_pair_to_index_map[byte_pair_frequencies_vec[i].first] = i;
            }

            // write the json to the file similar to case 2 but with 4 bytes instead of 2
            if(byte_pair_frequencies_vec.size() < UINT8_MAX){
                output_file << "{\n";
                // output_file << "        \"total_amount_of_unique_bytes\": " << byte_pair_frequencies_vec.size() << ",\n";
                output_file << "        \"" << binary_path_string << "\": {\n";
                for (auto it = byte_pair_to_index_map.begin(); it != byte_pair_to_index_map.end(); it++) {
                    output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first >> 8)) << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                    if (std::next(it) != byte_pair_to_index_map.end()) {
                        output_file << ",\n";
                    } else {
                        output_file << "\n";
                    }
                }
                output_file << "    }\n";
                output_file << "}\n\n";
            }

        }
        case 4:{
            // use int, int where the key is the first and second byte of the char array and the value is the frequency
            std::unordered_map<int, int> byte_pair_frequencies;
            for(int i = 0; i < binary_data_vec.size(); i+=4){
                int byte_pair = static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i])) << 24 | static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+1])) << 16 | static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+2])) << 8 | static_cast<unsigned int>(static_cast<uint8_t>(binary_data_vec[i+3]));
                byte_pair_frequencies[byte_pair]++;
            }

            std::ofstream output_file(json_path, std::ios::binary | std::ios::app);


            // remove all - from the path and replace them with _
            std::string binary_path_string = binary_path.string();
            std::replace(binary_path_string.begin(), binary_path_string.end(), '-', '_');
            // remove all / from the path and replace them with _
            std::replace(binary_path_string.begin(), binary_path_string.end(), '/', '_');
            // remove all . from the path and replace them with _
            std::replace(binary_path_string.begin(), binary_path_string.end(), '.', '_');
            // binary_path_string = binary_path_string + std::string{"_lookup_table"};

            // sort the map by value where value = 0 is the most frequent byte pair
            std::vector<std::pair<int, int>> byte_pair_frequencies_vec(byte_pair_frequencies.begin(), byte_pair_frequencies.end());
            std::sort(byte_pair_frequencies_vec.begin(), byte_pair_frequencies_vec.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b){
                return a.second > b.second;
            });

            if(byte_pair_frequencies_vec.size() < UINT16_MAX){
                output_file << "{\n";
                // output_file << "    \"" << binary_path_string << "\": {\n";
                // output_file << "        \"total_amount_of_unique_bytes\": " << byte_pair_frequencies_vec.size() << ",\n";;
                output_file << "        " << binary_path_string << ": {\n";
                for (auto it = byte_pair_frequencies_vec.begin(); it != byte_pair_frequencies_vec.end(); it++) {
                    output_file << "        \"" << static_cast<unsigned int>(static_cast<uint8_t>(it->first >> 24)) << static_cast<unsigned int>(static_cast<uint8_t>(it->first >> 16)) << static_cast<unsigned int>(static_cast<uint8_t>(it->first >> 8)) << static_cast<unsigned int>(static_cast<uint8_t>(it->first)) << "\": " << it->second;
                    if (std::next(it) != byte_pair_frequencies_vec.end()) {
                        output_file << ",\n";
                    } else {
                        output_file << "\n";
                    }
                }
                output_file << "    }\n";
                output_file << "}\n\n";
            }

            break;

        }
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }

        //if output file is empty then delete it
    std::filesystem::path output_file_path = json_path;
    std::ifstream output_file(output_file_path);
    if(output_file.peek() == std::ifstream::traits_type::eof()){
        output_file.close();
        std::filesystem::remove(output_file_path);
    }
}