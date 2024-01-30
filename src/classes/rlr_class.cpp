#include "rlr_class.hpp"
#include "alphabet_table.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <boost/multiprecision/cpp_int.hpp>
// #include "../PlanetData_LUT/PlanetData/Earth/Local/Hawaii.h"

#define ONE_NIBBLE_MAX 15
#define ONE_BYTE_MAX 255
#define ONE_BYTE_AND_ONE_NIBBLE_MAX 4095
#define TWO_BYTE_MAX 65535
#define TWO_BYTE_AND_ONE_NIBBLE_MAX 1048575
#define THREE_BYTE_MAX 16777215
#define THREE_BYTE_AND_ONE_NIBBLE_MAX 268435455
#define FOUR_BYTE_MAX 4294967295
#define FIVE_BYTE_MAX 1099511627775

constexpr size_t TWO_BYTE_COMBINATIONS = 65536;


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

constexpr std::array<std::pair<uint8_t, uint8_t>, 65536> Generate_Two_Byte_Alphabet() {
    std::array<std::pair<uint8_t, uint8_t>, 65536> alphabet{};
    int index = 0;
    for (uint16_t i = 0; i < 256; ++i) {
        for (uint16_t j = 0; j < 256; ++j) {
            alphabet[index++] = {static_cast<uint8_t>(i), static_cast<uint8_t>(j)};
        }
    }
    return alphabet;
}


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

// Helper Function for Run-Length Encoding
template<typename T>
void RLR::Run_Length_Encode(const std::vector<char>& data, std::vector<char>& encoded_data) {
    T current_byte = *reinterpret_cast<const T*>(data.data());
    uint8_t run_length = 1;
    for (size_t i = sizeof(T); i < data.size(); i += sizeof(T)) {
        T next_byte;
        std::memcpy(&next_byte, &data[i], sizeof(T));

        if (next_byte == current_byte && run_length < ONE_BYTE_MAX) {
            ++run_length;
        } else {
            encoded_data.push_back(static_cast<char>(run_length));
            encoded_data.insert(encoded_data.end(), reinterpret_cast<char*>(&current_byte), reinterpret_cast<char*>(&current_byte) + sizeof(T));
            current_byte = next_byte;
            run_length = 1;
        }
    }
    encoded_data.push_back(static_cast<char>(run_length));
    encoded_data.insert(encoded_data.end(), reinterpret_cast<char*>(&current_byte), reinterpret_cast<char*>(&current_byte) + sizeof(T));
}

// Helper Function for Run-Length Decoding
template<typename T>
void RLR::Run_Length_Decode(const std::vector<char>& encoded_data, std::vector<char>& decoded_data, size_t& write_index) {
    for (size_t i = 0; i < (encoded_data.size()); i += sizeof(T) + 1) {
        if (i + 1 + sizeof(T) > encoded_data.size()) {
            PRINT_DEBUG("Error: Invalid run-length encoded data.");
            ERROR_MSG_AND_EXIT("Size of encoded data:" + std::to_string(encoded_data.size()) + ", i: " + std::to_string(i) + ", sizeof(T): " + std::to_string(sizeof(T)));
        }

        uint8_t run_length = static_cast<uint8_t>(encoded_data[i]);
        T value;
        std::memcpy(&value, &encoded_data[i + 1], sizeof(T)); // Use memcpy for alignment-safe access

        std::fill_n(reinterpret_cast<T*>(&decoded_data[write_index]), run_length, value);
        write_index += run_length * sizeof(T);
    }
}


void RLR::Encode_With_One_Byte_Run_Length() {
    encoded_data_vec.clear();
    const size_t data_size = binary_data_vec.size();
    encoded_data_vec.reserve(data_size);

    switch (this->Get_Data_Type_Size()) {
        case 1:
            Run_Length_Encode<uint8_t>(binary_data_vec, encoded_data_vec);
            break;
        case 2:
            Run_Length_Encode<uint16_t>(binary_data_vec, encoded_data_vec);
            break;
        case 4:
            Run_Length_Encode<uint32_t>(binary_data_vec, encoded_data_vec);
            break;
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }
}

void RLR::Decode_With_One_Byte_Run_Length() {
    decoded_data_vec.clear();
    decoded_data_vec.resize(number_of_bytes_per_row);
    size_t write_index = 0;

    switch (this->Get_Data_Type_Size()) {
        case 1:
            Run_Length_Decode<uint8_t>(encoded_data_vec, decoded_data_vec, write_index);
            break;
        case 2:
            Run_Length_Decode<uint16_t>(encoded_data_vec, decoded_data_vec, write_index);
            break;
        case 4:
            Run_Length_Decode<uint32_t>(encoded_data_vec, decoded_data_vec, write_index);
            break;
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }
}

template<typename T>
std::vector<T> RLR::MTF_Encode_1Byte(const std::vector<T>& data) {
    std::list<uint8_t> mtf_list = ONE_BYTE_ALPHABET_VEC;
    std::vector<T> encoded_data;

    for (auto byte : data) {
        auto it = std::find(mtf_list.begin(), mtf_list.end(), byte);
        int index = std::distance(mtf_list.begin(), it);
        encoded_data.push_back(static_cast<T>(index));
        mtf_list.erase(it);
        mtf_list.push_front(byte);
    }

    return encoded_data;
}

// MTF Decoding for 1-byte elements
template<typename T>
std::vector<T> RLR::MTF_Decode_1Byte(const std::vector<T>& encoded_data) {
    std::list<uint8_t> mtf_list = ONE_BYTE_ALPHABET_VEC;
    std::vector<T> decoded_data;

    for (auto index : encoded_data) {
        if (index >= mtf_list.size()) {
            ERROR_MSG_AND_EXIT("Error: MTF index out of bounds.");
        }
        auto it = std::next(mtf_list.begin(), index);
        auto byte = *it;
        decoded_data.push_back(byte);
        mtf_list.erase(it);
        mtf_list.push_front(byte);
    }

    return decoded_data;
}

// MTF Encoding for 2-byte elements
template<typename T>
std::vector<T> RLR::MTF_Encode_2Byte(const std::vector<T>& data) {
    constexpr auto TWO_BYTE_ALPHABET = Generate_Two_Byte_Alphabet();
    std::list<std::pair<uint8_t, uint8_t>> mtf_list(TWO_BYTE_ALPHABET.begin(), TWO_BYTE_ALPHABET.end());
    std::vector<T> encoded_data;

    for (size_t i = 0; i < data.size(); i += 2) {
        std::pair<uint8_t, uint8_t> byte_pair(data[i], data[i + 1]);
        auto it = std::find(mtf_list.begin(), mtf_list.end(), byte_pair);
        int index = std::distance(mtf_list.begin(), it);

        // Store the index as two bytes
        encoded_data.push_back((index >> 8) & 0xFF); // Higher byte of index
        encoded_data.push_back(index & 0xFF);       // Lower byte of index

        // Move to front
        mtf_list.erase(it);
        mtf_list.push_front(byte_pair);
    }

    return encoded_data;
}

// MTF Decoding for 2-byte elements
template<typename T>
std::vector<T> RLR::MTF_Decode_2Byte(const std::vector<T>& encoded_data) {
    constexpr auto TWO_BYTE_ALPHABET = Generate_Two_Byte_Alphabet();
    std::list<std::pair<uint8_t, uint8_t>> mtf_list(TWO_BYTE_ALPHABET.begin(), TWO_BYTE_ALPHABET.end());
    std::vector<T> decoded_data;

    for (size_t i = 0; i < encoded_data.size(); i += 2) {
        unsigned int highByte = static_cast<unsigned int>(static_cast<unsigned char>(encoded_data[i]));
        unsigned int lowByte = static_cast<unsigned int>(static_cast<unsigned char>(encoded_data[i + 1]));
        int index = (highByte << 8) | lowByte;

        // std::cout << "High byte: " << highByte << ", Low byte: " << lowByte << ", Index: " << index << ", MTF List Size: " << mtf_list.size() << std::endl;

        if (index < 0 || index >= static_cast<int>(mtf_list.size())) {
            ERROR_MSG_AND_EXIT("Error: MTF index out of bounds.");
        }

        auto it = std::next(mtf_list.begin(), index);
        std::pair<uint8_t, uint8_t> byte_pair = *it;

        decoded_data.push_back(byte_pair.first);
        decoded_data.push_back(byte_pair.second);

        // Move the byte pair to the front of the MTF list
        mtf_list.erase(it);
        mtf_list.push_front(byte_pair);
    }

    return decoded_data;
}


void RLR::Encode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length() {
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    std::vector<char> rlr_encoded_data_vec;
    rlr_encoded_data_vec.reserve(binary_data_vec.size());

    switch(data_type_size) {
        case 1: {
            auto mtf_encoded_data = MTF_Encode_1Byte(binary_data_vec);
            Run_Length_Encode<uint8_t>(mtf_encoded_data, rlr_encoded_data_vec);
            break;
        }
        case 2: {

            // constexpr auto TWO_BYTE_ALPHABET = Generate_Two_Byte_Alphabet();
            auto mtf_encoded_data = MTF_Encode_2Byte(binary_data_vec);
            Run_Length_Encode<u_int16_t>(mtf_encoded_data, rlr_encoded_data_vec);
            break;
        }
        case 4: {
            // Run_Length_Encode<uint8_t>(mtf_encoded_data, rlr_encoded_data_vec);
            Run_Length_Encode<uint32_t>(binary_data_vec, rlr_encoded_data_vec);
            break;
        }
        default: {
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
        }
    }

    encoded_data_vec = std::move(rlr_encoded_data_vec);
    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length() {
    std::vector<char> rlr_decoded_data_vec(number_of_bytes_per_row);
    size_t write_index = 0;

    switch (this->Get_Data_Type_Size()) {
        case 1:
            Run_Length_Decode<uint8_t>(encoded_data_vec, rlr_decoded_data_vec, write_index);
            break;
        case 2:
            Run_Length_Decode<uint16_t>(encoded_data_vec, rlr_decoded_data_vec, write_index);
            break;
        case 4:
            Run_Length_Decode<uint32_t>(encoded_data_vec, rlr_decoded_data_vec, write_index);
            break;
        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }

    decoded_data_vec.clear();
    decoded_data_vec.reserve(rlr_decoded_data_vec.size());

    std::list<uint8_t> mtf_list;

    switch (this->Get_Data_Type_Size()) {
        case 1: {
            mtf_list = ONE_BYTE_ALPHABET_VEC;

            for (uint8_t index : rlr_decoded_data_vec) {
                if (index >= mtf_list.size()) {
                    ERROR_MSG_AND_EXIT("Error: MTF index out of bounds.");
                    break;
                }
                auto it = std::next(mtf_list.begin(), index);
                uint8_t byte = *it;
                decoded_data_vec.push_back(byte);
                mtf_list.erase(it);
                mtf_list.push_front(byte);
            }
            break;
        }
        case 2: {
            constexpr auto TWO_BYTE_ALPHABET = Generate_Two_Byte_Alphabet();
            std::list<std::pair<uint8_t, uint8_t>> mtf_list(TWO_BYTE_ALPHABET.begin(), TWO_BYTE_ALPHABET.end());

            for (size_t i = 0; i < rlr_decoded_data_vec.size(); i += 2) {
                unsigned int highByte = static_cast<unsigned int>(static_cast<unsigned char>(rlr_decoded_data_vec[i]));
                unsigned int lowByte = static_cast<unsigned int>(static_cast<unsigned char>(rlr_decoded_data_vec[i + 1]));
                int index = (highByte << 8) | lowByte;

                if (index < 0 || index >= static_cast<int>(mtf_list.size())) {
                    ERROR_MSG_AND_EXIT("Error: MTF index out of bounds.");
                    break;
                }

                auto it = std::next(mtf_list.begin(), index);
                std::pair<uint8_t, uint8_t> byte_pair = *it;

                decoded_data_vec.push_back(byte_pair.first);
                decoded_data_vec.push_back(byte_pair.second);

                // Move the byte pair to the front of the MTF list
                mtf_list.erase(it);
                mtf_list.push_front(byte_pair);
            }
            break;
        }
        default:
            break;
    }
}


// burrow wheeler transform with move to front transformation then run length encoding


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
const char* RLR::Get_Compression_Type() const {
    return compression_type;
}

const std::vector<char>& RLR::Get_Encoded_Data_Vec() const {
    return encoded_data_vec;
}

const std::vector<char>& RLR::Get_Decoded_Data_Vec() const {
    return decoded_data_vec;
}

const std::vector<char>& RLR::Get_Binary_Data_Vec() const {
    return binary_data_vec;
}