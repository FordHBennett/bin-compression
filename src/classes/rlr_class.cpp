#include "rlr_class.hpp"
#include "alphabet_table.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>

#define ONE_NIBBLE_MAX 15
#define ONE_BYTE_MAX 255
#define ONE_BYTE_AND_ONE_NIBBLE_MAX 4095
#define TWO_BYTE_MAX 65535
#define TWO_BYTE_AND_ONE_NIBBLE_MAX 1048575
#define THREE_BYTE_MAX 16777215
#define THREE_BYTE_AND_ONE_NIBBLE_MAX 268435455
#define FOUR_BYTE_MAX 4294967295
#define FIVE_BYTE_MAX 1099511627775


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

//Constructors
RLR::RLR(){}


void RLR::Read_File(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row) {
    // clear the binary data vector
    binary_data_vec.clear();

    std::ifstream input_file(file_path, std::ios::binary);
#ifdef DEBUG
    if(!input_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }
#endif

    binary_data_vec.resize(number_of_bytes_to_read);
    // populate the binary data vector with the data from the file starting at number_of_bytes_to_read * row
    std::streampos start_position = static_cast<std::streampos>(number_of_bytes_to_read) * row;
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
}

void RLR::Encode_With_One_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    size_t byte_index = 0;

    switch(data_type_size){
        case 1:{
            char current_byte = binary_data_vec[0];
            uint8_t run_length = 1;
            for(size_t i = 1; i < binary_data_vec.size(); i++){
                if(binary_data_vec[i] == current_byte && run_length < ONE_BYTE_MAX){
                    run_length++;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.push_back(current_byte);
                    // current_byte = binary_data_vec[i];
                    memcpy(&current_byte, &binary_data_vec[i], 1);
                    run_length = 1;
                }
            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.push_back(current_byte);
            break;
        }
        case 2:{
            std::array<char, 2> current_byte = {binary_data_vec[0], binary_data_vec[1]};
            uint8_t run_length = 1;
            for(size_t i = 2; i < binary_data_vec.size(); i += 2){
                if(std::equal(binary_data_vec.begin() + i, binary_data_vec.begin() + i + 2, current_byte.begin()) && run_length < ONE_BYTE_MAX){
                    run_length++;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.insert(encoded_data_vec.end(), current_byte.begin(), current_byte.end());

                    memcpy(current_byte.data(), &binary_data_vec[i], 2);
                    run_length = 1;
                }

            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.insert(encoded_data_vec.end(), current_byte.begin(), current_byte.end());
            break;
        }

        case 4:{
            std::array<char, 4> current_byte = {binary_data_vec[0], binary_data_vec[1], binary_data_vec[2], binary_data_vec[3]};
            uint8_t run_length = 1;
            for(size_t i = 4; i < binary_data_vec.size(); i += 4){
                if(std::equal(binary_data_vec.begin() + i, binary_data_vec.begin() + i + 4, current_byte.begin()) && run_length < ONE_BYTE_MAX){
                    run_length++;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.insert(encoded_data_vec.end(), current_byte.begin(), current_byte.end());
                    memcpy(current_byte.data(), &binary_data_vec[i], 4);
                    run_length = 1;
                }
            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.insert(encoded_data_vec.end(), current_byte.begin(), current_byte.end());
            break;
        }

        default:{
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
        }
    }

    encoded_data_vec.shrink_to_fit();
}


void RLR::Decode_With_One_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    size_t write_index = 0;
    decoded_data_vec.resize(number_of_bytes_per_row);  // Assuming number_of_bytes_per_row is correctly set

    switch(data_type_size) {
        case 1: {
            for(size_t i = 0; i < encoded_data_vec.size(); i += 2) {
                // uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                memset(&decoded_data_vec[write_index], encoded_data_vec[i + 1], static_cast<uint8_t>(encoded_data_vec[i]));
                write_index += static_cast<uint8_t>(encoded_data_vec[i]);
            }
            break;
        }

        case 2: {
            for(size_t i = 0; i < encoded_data_vec.size(); i += 3) {
                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                std::array<char, 2> current_byte = {encoded_data_vec[i + 1], encoded_data_vec[i + 2]};
                for(size_t j = 0; j < run_length; j++) {
                    std::memcpy(&decoded_data_vec[write_index], current_byte.data(), 2);
                    write_index += 2;
                }
            }
            break;
        }
        case 4: {
            for(size_t i = 0; i < encoded_data_vec.size(); i += 5) {
                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                std::array<char, 4> current_byte = {encoded_data_vec[i + 1], encoded_data_vec[i + 2], encoded_data_vec[i + 3], encoded_data_vec[i + 4]};
                for(size_t j = 0; j < run_length; j++) {
                    std::memcpy(&decoded_data_vec[write_index], current_byte.data(), 4);
                    write_index += 4;
                }


            }
            break;
        }

        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }
}



void RLR::Write_Compressed_File(const std::filesystem::path& file_path) const {
    try {

        std::ofstream encoded_output_file(file_path, std::ios::binary | std::ios::app);

        encoded_output_file.write(encoded_data_vec.data(), encoded_data_vec.size());

        encoded_output_file.close();
    } catch (const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error: Unable to create the run-length encoded file.\n"} + std::string{"ERROR CODE: "} + std::string{e.what()});
    }
}

void RLR::Write_Decompressed_File(const std::filesystem::path& file_path) const {
    try{
        std::ofstream decoded_output_file(file_path, std::ios::binary | std::ios::app);

        decoded_output_file.write(decoded_data_vec.data(), decoded_data_vec.size());

        decoded_output_file.close();
    } catch (const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error: Unable to create the run-length decoded file.\n"} + std::string{"ERROR CODE: "} + std::string{e.what()});
    }
}



//getters
const char* RLR::Get_Compression_Type() const {return compression_type;}

const std::vector<char> RLR::Get_Encoded_Data_Vec() const {return encoded_data_vec;}

const std::vector<char> RLR::Get_Decoded_Data_Vec() const {return decoded_data_vec;}

const std::vector<char> RLR::Get_Binary_Data_Vec() const {return binary_data_vec;}