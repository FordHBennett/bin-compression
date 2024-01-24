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

void RLR::Encode_With_One_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    const size_t data_size = binary_data_vec.size();
    encoded_data_vec.reserve(data_size); // reserve based on expected size
    size_t byte_index = 0;

    switch(data_type_size) {
        case 1: {
            char current_byte = binary_data_vec[0];
            uint8_t run_length = 1;
            for(size_t i = 1; i < data_size; ++i) {
                if(binary_data_vec[i] == current_byte && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.push_back(current_byte);
                    current_byte = binary_data_vec[i];
                    run_length = 1;
                }
            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.push_back(current_byte);
            break;
        }
        case 2: {
            char current_bytes[2] = {binary_data_vec[0], binary_data_vec[1]};
            uint8_t run_length = 1;
            for(size_t i = 2; i < data_size; i += 2) {
                if((binary_data_vec[i] == current_bytes[0] && binary_data_vec[i + 1] == current_bytes[1]) && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.push_back(current_bytes[0]);
                    encoded_data_vec.push_back(current_bytes[1]);
                    current_bytes[0] = binary_data_vec[i];
                    current_bytes[1] = binary_data_vec[i + 1];
                    run_length = 1;
                }
            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.push_back(current_bytes[0]);
            encoded_data_vec.push_back(current_bytes[1]);
            break;
        }
        case 4: {
            char current_bytes[4] = {binary_data_vec[0], binary_data_vec[1], binary_data_vec[2], binary_data_vec[3]};
            uint8_t run_length = 1;
            for(size_t i = 4; i < data_size; i += 4) {
                if((binary_data_vec[i] == current_bytes[0] && binary_data_vec[i + 1] == current_bytes[1] && binary_data_vec[i + 2] == current_bytes[2] && binary_data_vec[i + 3] == current_bytes[3]) && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    encoded_data_vec.push_back(static_cast<char>(run_length));
                    encoded_data_vec.push_back(current_bytes[0]);
                    encoded_data_vec.push_back(current_bytes[1]);
                    encoded_data_vec.push_back(current_bytes[2]);
                    encoded_data_vec.push_back(current_bytes[3]);
                    current_bytes[0] = binary_data_vec[i];
                    current_bytes[1] = binary_data_vec[i + 1];
                    current_bytes[2] = binary_data_vec[i + 2];
                    current_bytes[3] = binary_data_vec[i + 3];
                    run_length = 1;
                }
            }
            encoded_data_vec.push_back(static_cast<char>(run_length));
            encoded_data_vec.push_back(current_bytes[0]);
            encoded_data_vec.push_back(current_bytes[1]);
            encoded_data_vec.push_back(current_bytes[2]);
            encoded_data_vec.push_back(current_bytes[3]);
            break;
        }
        default: {
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
        }
    }

    encoded_data_vec.shrink_to_fit();
}


void RLR::Decode_With_One_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    const size_t encoded_size = encoded_data_vec.size();
    size_t write_index = 0;
    decoded_data_vec.resize(number_of_bytes_per_row);  // Assuming this is correctly set

    switch(data_type_size) {
        case 1: {
            for(size_t i = 0; i < encoded_size; i += 2) {
                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                std::memset(&decoded_data_vec[write_index], encoded_data_vec[i + 1], run_length);
                write_index += run_length;
            }
            break;
        }

        case 2: {
            for(size_t i = 0; i < encoded_size; i += 3) {
                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                char current_bytes[2] = {encoded_data_vec[i + 1], encoded_data_vec[i + 2]};
                for(uint8_t j = 0; j < run_length; ++j) {
                    decoded_data_vec[write_index] = current_bytes[0];
                    decoded_data_vec[write_index + 1] = current_bytes[1];
                    write_index += 2;
                }
            }
            break;
        }

        case 4: {
            for(size_t i = 0; i < encoded_size; i += 5) {
                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[i]);
                char current_bytes[4] = {encoded_data_vec[i + 1], encoded_data_vec[i + 2], encoded_data_vec[i + 3], encoded_data_vec[i + 4]};
                for(uint8_t j = 0; j < run_length; ++j) {
                    std::memcpy(&decoded_data_vec[write_index], current_bytes, 4);
                    write_index += 4;
                }
            }
            break;
        }

        default:
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
    }

    decoded_data_vec.shrink_to_fit();
}

void RLR::Encode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length() {
    // Initialize the Move-To-Front list with all possible byte values (0-255)

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    const size_t data_size = binary_data_vec.size();
    size_t byte_index = 0;
    std::vector<char> rlr_encoded_data_vec;
    rlr_encoded_data_vec.reserve(data_size);
    encoded_data_vec.clear();


    switch(data_type_size) {
        case 1: {
            std::list<uint8_t> mtf_list = ONE_BYTE_ALPHABET_VEC;
            // for (int i = 0; i < 256; ++i) {
            //     mtf_list.push_back(i);
            // }

            for (uint8_t byte : binary_data_vec) {
                // Find the position of the current byte in the MTF list
                auto it = std::find(mtf_list.begin(), mtf_list.end(), byte);
                int index = std::distance(mtf_list.begin(), it);

                // Add the index to the encoded data
                encoded_data_vec.push_back(static_cast<uint8_t>(index));

                // Move the byte to the front of the MTF list
                mtf_list.erase(it);
                mtf_list.push_front(byte);
            }

            char current_byte = encoded_data_vec[0];
            uint8_t run_length = 1;
            for(size_t i = 1; i < data_size; ++i) {
                if(encoded_data_vec[i] == current_byte && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_byte);
                    current_byte = encoded_data_vec[i];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_byte);
            break;
        }
        case 2: {

            constexpr auto TWO_BYTE_ALPHABET = Generate_Two_Byte_Alphabet();

            std::list<std::pair<uint8_t, uint8_t>> mtf_list(TWO_BYTE_ALPHABET.begin(), TWO_BYTE_ALPHABET.end());

            for (size_t i = 0; i < data_size; i += 2) {
                std::pair<uint8_t, uint8_t> byte_pair(binary_data_vec[i], binary_data_vec[i + 1]);
                auto it = std::find(mtf_list.begin(), mtf_list.end(), byte_pair);
                int index = std::distance(mtf_list.begin(), it);

                // Store the index as two bytes
                encoded_data_vec.push_back((index >> 8) & 0xFF); // Higher byte of index
                encoded_data_vec.push_back(index & 0xFF);       // Lower byte of index

                // Move to front
                mtf_list.erase(it);
                mtf_list.push_front(byte_pair);
            }

            char current_bytes[2] = {encoded_data_vec[0], encoded_data_vec[1]};
            uint8_t run_length = 1;
            for(size_t i = 2; i < data_size; i += 2) {
                if((encoded_data_vec[i] == current_bytes[0] && encoded_data_vec[i + 1] == current_bytes[1]) && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_bytes[0]);
                    rlr_encoded_data_vec.push_back(current_bytes[1]);
                    current_bytes[0] = encoded_data_vec[i];
                    current_bytes[1] = encoded_data_vec[i + 1];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_bytes[0]);
            rlr_encoded_data_vec.push_back(current_bytes[1]);
            break;
        }
        case 4: {
            char current_bytes[4] = {encoded_data_vec[0], encoded_data_vec[1], encoded_data_vec[2], encoded_data_vec[3]};
            uint8_t run_length = 1;
            for(size_t i = 4; i < data_size; i += 4) {
                if((encoded_data_vec[i] == current_bytes[0] && encoded_data_vec[i + 1] == current_bytes[1] && encoded_data_vec[i + 2] == current_bytes[2] && encoded_data_vec[i + 3] == current_bytes[3]) && run_length < ONE_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_bytes[0]);
                    rlr_encoded_data_vec.push_back(current_bytes[1]);
                    rlr_encoded_data_vec.push_back(current_bytes[2]);
                    rlr_encoded_data_vec.push_back(current_bytes[3]);
                    current_bytes[0] = encoded_data_vec[i];
                    current_bytes[1] = encoded_data_vec[i + 1];
                    current_bytes[2] = encoded_data_vec[i + 2];
                    current_bytes[3] = encoded_data_vec[i + 3];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_bytes[0]);
            rlr_encoded_data_vec.push_back(current_bytes[1]);
            rlr_encoded_data_vec.push_back(current_bytes[2]);
            rlr_encoded_data_vec.push_back(current_bytes[3]);
            break;
        }
        default: {
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
        }
    }

    //assign the rlr_encoded_data_vec to the encoded_data_vec
    // encoded_data_vec = rlr_encoded_data_vec;
    encoded_data_vec = std::move(rlr_encoded_data_vec);
    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length() {
        if (encoded_data_vec.empty()) {
        ERROR_MSG_AND_EXIT("Error: Encoded data vector is empty.");
        return;
    }

    std::vector<char> rlr_decoded_data_vec;
    rlr_decoded_data_vec.reserve(encoded_data_vec.size());

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    size_t read_index = 0;

    while (read_index < encoded_data_vec.size()) {
        switch(data_type_size) {
            case 1: {
                if (read_index + 1 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]);
                char value = encoded_data_vec[read_index + 1];
                rlr_decoded_data_vec.insert(rlr_decoded_data_vec.end(), run_length, value);
                read_index += 2;
                break;
            }
            case 2: {
                if (read_index + 2 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]);
                char value1 = encoded_data_vec[read_index + 1];
                char value2 = encoded_data_vec[read_index + 2];
                for (uint8_t i = 0; i < run_length; ++i) {
                    rlr_decoded_data_vec.push_back(value1);
                    rlr_decoded_data_vec.push_back(value2);
                }
                read_index += 3;
                break;
            }
            case 4: {
                if (read_index + 4 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]);
                char value1 = encoded_data_vec[read_index + 1];
                char value2 = encoded_data_vec[read_index + 2];
                char value3 = encoded_data_vec[read_index + 3];
                char value4 = encoded_data_vec[read_index + 4];
                for (uint8_t i = 0; i < run_length; ++i) {
                    rlr_decoded_data_vec.push_back(value1);
                    rlr_decoded_data_vec.push_back(value2);
                    rlr_decoded_data_vec.push_back(value3);
                    rlr_decoded_data_vec.push_back(value4);
                }
                read_index += 5;
                break;
            }
            default: {
                ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
                break;
            }
        }
    }

    decoded_data_vec.clear();
    decoded_data_vec.reserve(rlr_decoded_data_vec.size());

    std::list<uint8_t> mtf_list;

    switch (data_type_size) {
        case 1:{
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

                // std::cout << "High byte: " << highByte << ", Low byte: " << lowByte << ", Index: " << index << ", MTF List Size: " << mtf_list.size() << std::endl;

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

void RLR::Encode_With_Move_To_Front_Transformation_With_Two_Byte_Run_Length(){
    // Initialize the Move-To-Front list with all possible byte values (0-255)
    std::list<uint8_t> mtf_list;
    for (int i = 0; i < 256; ++i) {
        mtf_list.push_back(i);
    }

    encoded_data_vec.clear();
    for (uint8_t byte : binary_data_vec) {
        // Find the position of the current byte in the MTF list
        auto it = std::find(mtf_list.begin(), mtf_list.end(), byte);
        int index = std::distance(mtf_list.begin(), it);

        // Add the index to the encoded data
        encoded_data_vec.push_back(static_cast<uint8_t>(index));

        // Move the byte to the front of the MTF list
        mtf_list.erase(it);
        mtf_list.push_front(byte);
    }

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    const size_t data_size = binary_data_vec.size();
    size_t byte_index = 0;
    std::vector<char> rlr_encoded_data_vec;
    rlr_encoded_data_vec.reserve(data_size);

    switch(data_type_size) {
        case 1: {
            char current_byte = encoded_data_vec[0];
            uint8_t run_length = 1;
            for(size_t i = 1; i < data_size; ++i) {
                if(encoded_data_vec[i] == current_byte && run_length < TWO_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_byte);
                    current_byte = encoded_data_vec[i];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_byte);
            break;
        }
        case 2: {
            char current_bytes[2] = {encoded_data_vec[0], encoded_data_vec[1]};
            uint8_t run_length = 1;
            for(size_t i = 2; i < data_size; i += 2) {
                if((encoded_data_vec[i] == current_bytes[0] && encoded_data_vec[i + 1] == current_bytes[1]) && run_length < TWO_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_bytes[0]);
                    rlr_encoded_data_vec.push_back(current_bytes[1]);
                    current_bytes[0] = encoded_data_vec[i];
                    current_bytes[1] = encoded_data_vec[i + 1];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_bytes[0]);
            rlr_encoded_data_vec.push_back(current_bytes[1]);
            break;
        }
        case 4: {
            char current_bytes[4] = {encoded_data_vec[0], encoded_data_vec[1], encoded_data_vec[2], encoded_data_vec[3]};
            uint8_t run_length = 1;
            for(size_t i = 4; i < data_size; i += 4) {
                if((encoded_data_vec[i] == current_bytes[0] && encoded_data_vec[i + 1] == current_bytes[1] && encoded_data_vec[i + 2] == current_bytes[2] && encoded_data_vec[i + 3] == current_bytes[3]) && run_length < TWO_BYTE_MAX) {
                    ++run_length;
                } else {
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
                    rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
                    rlr_encoded_data_vec.push_back(current_bytes[0]);
                    rlr_encoded_data_vec.push_back(current_bytes[1]);
                    rlr_encoded_data_vec.push_back(current_bytes[2]);
                    rlr_encoded_data_vec.push_back(current_bytes[3]);
                    current_bytes[0] = encoded_data_vec[i];
                    current_bytes[1] = encoded_data_vec[i + 1];
                    current_bytes[2] = encoded_data_vec[i + 2];
                    current_bytes[3] = encoded_data_vec[i + 3];
                    run_length = 1;
                }
            }
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
            rlr_encoded_data_vec.push_back(static_cast<char>(run_length));
            rlr_encoded_data_vec.push_back(current_bytes[0]);
            rlr_encoded_data_vec.push_back(current_bytes[1]);
            rlr_encoded_data_vec.push_back(current_bytes[2]);
            rlr_encoded_data_vec.push_back(current_bytes[3]);
            break;
        }
        default: {
            ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
            break;
        }
    }

    //assign the rlr_encoded_data_vec to the encoded_data_vec
    // encoded_data_vec = rlr_encoded_data_vec;
    encoded_data_vec = std::move(rlr_encoded_data_vec);
    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Move_To_Front_Transformation_With_Two_Byte_Run_Length(){
    if (encoded_data_vec.empty()) {
        ERROR_MSG_AND_EXIT("Error: Encoded data vector is empty.");
        return;
    }

    std::vector<char> rlr_decoded_data_vec;
    rlr_decoded_data_vec.reserve(encoded_data_vec.size());

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    size_t read_index = 0;

    while (read_index < encoded_data_vec.size()) {
        switch(data_type_size) {
            case 1: {
                if (read_index + 2 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]) << 8 | static_cast<uint8_t>(encoded_data_vec[read_index + 1]);
                char value = encoded_data_vec[read_index + 2];
                rlr_decoded_data_vec.insert(rlr_decoded_data_vec.end(), run_length, value);
                read_index += 3;
                break;
            }
            case 2: {
                if (read_index + 3 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]) << 8 | static_cast<uint8_t>(encoded_data_vec[read_index + 1]);
                char value1 = encoded_data_vec[read_index + 2];
                char value2 = encoded_data_vec[read_index + 3];
                for (uint8_t i = 0; i < run_length; ++i) {
                    rlr_decoded_data_vec.push_back(value1);
                    rlr_decoded_data_vec.push_back(value2);
                }
                read_index += 4;
                break;
            }
            case 4: {
                if (read_index + 5 >= encoded_data_vec.size()) break;

                uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[read_index]) << 8 | static_cast<uint8_t>(encoded_data_vec[read_index + 1]);
                char value1 = encoded_data_vec[read_index + 2];
                char value2 = encoded_data_vec[read_index + 3];
                char value3 = encoded_data_vec[read_index + 4];
                char value4 = encoded_data_vec[read_index + 5];
                for (uint8_t i = 0; i < run_length; ++i) {
                    rlr_decoded_data_vec.push_back(value1);
                    rlr_decoded_data_vec.push_back(value2);
                    rlr_decoded_data_vec.push_back(value3);
                    rlr_decoded_data_vec.push_back(value4);
                }
                read_index += 6;
                break;
            }
            default: {
                ERROR_MSG_AND_EXIT("Error: Invalid data type size.");
                break;
            }
        }
    }

    decoded_data_vec.shrink_to_fit();
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
const char* RLR::Get_Compression_Type() const {return compression_type;}

const std::vector<char> RLR::Get_Encoded_Data_Vec() const {return encoded_data_vec;}

const std::vector<char> RLR::Get_Decoded_Data_Vec() const {return decoded_data_vec;}

const std::vector<char> RLR::Get_Binary_Data_Vec() const {return binary_data_vec;}