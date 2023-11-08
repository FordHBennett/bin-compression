#include "rlr_class.hpp"
#include <fstream>
#include <iostream>
#include <vector>

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
    if(!input_file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open the file.");
    }

    binary_data_vec.resize(number_of_bytes_to_read);
    // populate the binary data vector with the data from the file starting at number_of_bytes_to_read * row
    std::streampos start_position = static_cast<std::streampos>(number_of_bytes_to_read) * row;
    input_file.seekg(start_position);

    if (input_file.fail()) {
        ERROR_MSG_AND_EXIT("Error: Unable to seek to the specified position in the file.");
    }

    input_file.close();
}

void RLR::Encode_With_One_Byte_Run_Length() {
    // Clear the encoded data vector
    encoded_data_vec.clear();

    const int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size() / 2);
    int byte_index = 0;
    std::vector<char> current_block;
    current_block.reserve(data_type_size);

    while (byte_index < binary_data_vec.size()) {
        uint8_t run_length = 1;
        int next_index = byte_index + data_type_size;

        current_block.assign(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

        while (next_index < binary_data_vec.size() && run_length < 255) {
            if (std::equal(current_block.begin(), current_block.end(),
                           binary_data_vec.begin() + next_index)) {
                run_length++;
                next_index += data_type_size;
            } else {
                break;
            }
        }

        encoded_data_vec.push_back(static_cast<char>(run_length));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_One_Byte_Run_Length() {
    // Clear the vector to make sure we're starting fresh.
    decoded_data_vec.clear();

    decoded_data_vec.reserve(binary_data_vec.size());

    const int data_type_size = this->Get_Data_Type_Size();
    auto it = encoded_data_vec.begin();

    while(it != encoded_data_vec.end()) {
        uint8_t run_length = static_cast<uint8_t>(*it++);

        for (int run = 0; run < run_length; ++run) {
            decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
        }

        it += data_type_size;
    }
}

// why does this work?
void RLR::Encode_With_Two_Byte_Run_Length() {
    // Clear the encoded data vector
    encoded_data_vec.clear();

    int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size() / 2);
    int byte_index = 0;

    while (byte_index < binary_data_vec.size()) {
        uint16_t run_length = 1;
        int next_index = byte_index + data_type_size;

        std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

        while (next_index < binary_data_vec.size() && run_length < 65535) {
            if (std::equal(current_block.begin(), current_block.end(),
                           binary_data_vec.begin() + next_index)) {
                run_length++;
                next_index += data_type_size;
            } else {
                break;
            }
        }

        encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<char>(run_length));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

// why does this work?
void RLR::Decode_With_Two_Byte_Run_Length() {
    // Clear the vector to make sure we're starting fresh.
    decoded_data_vec.clear();

    decoded_data_vec.reserve(binary_data_vec.size());

    int data_type_size = this->Get_Data_Type_Size();
    auto it = encoded_data_vec.begin();

    while(it != encoded_data_vec.end()) {
        uint16_t run_length = static_cast<uint8_t>(*it++) << 8;
        run_length |= static_cast<uint8_t>(*it++);

        for (int run = 0; run < run_length; ++run) {
            decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
        }

        it += data_type_size;
    }
}

//FASTER
void RLR::Encode_With_One_Nibble_Run_Length() {
    encoded_data_vec.clear();

    const int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size() / 5);
    int byte_index = 0;
    std::vector<char> current_block;
    current_block.reserve(data_type_size);

    while (byte_index < binary_data_vec.size()) {
        uint8_t first_nibble_run_length = 1;
        uint8_t second_nibble_run_length = 0;
        int next_index = byte_index + data_type_size;

        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while (next_index < binary_data_vec.size() && first_nibble_run_length < 15) {
            if (std::equal(current_block.begin(), current_block.end(),
                           binary_data_vec.begin() + next_index)) {
                first_nibble_run_length++;
                next_index += data_type_size;
            } else {
                break;
            }
        }

        byte_index = next_index; // Update the byte_index to the next_index for the next iteration

        // Check if there is more data to process for the second nibble
        if (byte_index < binary_data_vec.size()) {
            current_block.assign(binary_data_vec.begin() + byte_index,
                                 binary_data_vec.begin() + byte_index + data_type_size);

            while (next_index < binary_data_vec.size() && second_nibble_run_length < 15) {
                if (std::equal(current_block.begin(), current_block.end(),
                               binary_data_vec.begin() + next_index)) {
                    second_nibble_run_length++;
                    next_index += data_type_size;
                } else {
                    break;
                }
            }
        }

        byte_index = next_index; // Update the byte_index to the next_index for the next iteration

        // Pack the two run lengths into one byte
        uint8_t packed_run_lengths = (first_nibble_run_length << 4) | second_nibble_run_length;
        encoded_data_vec.push_back(static_cast<char>(packed_run_lengths));
        if (first_nibble_run_length > 0) {
            encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());
        }
        if (second_nibble_run_length > 0) {
            encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());
        }
    }

    encoded_data_vec.shrink_to_fit();
}


//SLOWER
// void RLR::Encode_With_One_Nibble_Run_Length() {
//     encoded_data_vec.clear();

//     const int data_type_size = this->Get_Data_Type_Size();
//     encoded_data_vec.reserve(binary_data_vec.size());

//     size_t byte_index = 0;

//     while (byte_index < binary_data_vec.size()) {
//         const char* current_block = &binary_data_vec[byte_index];
//         uint8_t first_nibble_run_length = 1;
//         uint8_t second_nibble_run_length = 0;

//         while ((byte_index + first_nibble_run_length * data_type_size < binary_data_vec.size()) &&
//                (memcmp(current_block, &binary_data_vec[byte_index + first_nibble_run_length * data_type_size], data_type_size) == 0) &&
//                (first_nibble_run_length < 15)) {
//             ++first_nibble_run_length;
//         }

//         size_t current_block_end_index = byte_index + first_nibble_run_length * data_type_size;

//         // Calculate the second run length if there's more data
//         if (current_block_end_index < binary_data_vec.size()) {
//             current_block = &binary_data_vec[current_block_end_index];
//             while ((current_block_end_index + second_nibble_run_length * data_type_size < binary_data_vec.size()) &&
//                    (memcmp(current_block, &binary_data_vec[current_block_end_index + second_nibble_run_length * data_type_size], data_type_size) == 0) &&
//                    (second_nibble_run_length < 15)) {
//                 ++second_nibble_run_length;
//             }
//         }


//         uint8_t packed_run_lengths = (first_nibble_run_length << 4) | second_nibble_run_length;
//         encoded_data_vec.push_back(static_cast<char>(packed_run_lengths));

//         size_t new_size = encoded_data_vec.size() + data_type_size;
//         encoded_data_vec.resize(new_size);
//         memcpy(encoded_data_vec.data() + new_size - data_type_size, current_block, data_type_size);
//         byte_index += first_nibble_run_length * data_type_size;

//         if (second_nibble_run_length > 0) {
//             new_size = encoded_data_vec.size() + data_type_size;
//             encoded_data_vec.resize(new_size);
//             memcpy(encoded_data_vec.data() + new_size - data_type_size, current_block, data_type_size);
//             byte_index += second_nibble_run_length * data_type_size;
//         }
//     }


//     encoded_data_vec.shrink_to_fit();
// }

//FASTER
void RLR::Decode_With_One_Nibble_Run_Length() {
    decoded_data_vec.clear();

    const int data_type_size = this->Get_Data_Type_Size();
    decoded_data_vec.reserve(binary_data_vec.size());

    int byte_index = 0;
    while (byte_index < encoded_data_vec.size()) {
        uint8_t packed_run_lengths = static_cast<uint8_t>(encoded_data_vec[byte_index++]);
        uint8_t first_nibble_run_length = (packed_run_lengths >> 4);
        uint8_t second_nibble_run_length = packed_run_lengths & 0x0F;


        int new_size = decoded_data_vec.size() + (first_nibble_run_length + second_nibble_run_length) * data_type_size;
        decoded_data_vec.resize(new_size);


        char* insert_position = decoded_data_vec.data() + decoded_data_vec.size() - (first_nibble_run_length + second_nibble_run_length) * data_type_size;


        for (int run = 0; run < first_nibble_run_length; run++) {
            memcpy(insert_position, &encoded_data_vec[byte_index], data_type_size);
            insert_position += data_type_size;
        }
        byte_index += data_type_size;

        if ((second_nibble_run_length > 0) && (byte_index < encoded_data_vec.size())) {
            for (int run = 0; run < second_nibble_run_length; run++) {
                memcpy(insert_position, &encoded_data_vec[byte_index], data_type_size);
                insert_position += data_type_size;
            }
            byte_index += data_type_size;
        }
    }
}

//SLOWER
// void RLR::Decode_With_One_Nibble_Run_Length() {
//     decoded_data_vec.clear();

//     // Assuming the encoded data contains at least one data block after each byte of run lengths
//     const int data_type_size = this->Get_Data_Type_Size();
//     // Reserve some space upfront to reduce allocations
//     decoded_data_vec.reserve(encoded_data_vec.size() * data_type_size);

//     // Iterate through each byte of run length information
//     for (size_t i = 0; i < encoded_data_vec.size(); ) {
//         // Extract run lengths from the current byte
//         uint8_t packed_run_lengths = static_cast<uint8_t>(encoded_data_vec[i]);
//         uint8_t first_nibble_run_length = (packed_run_lengths >> 4) & 0x0F;  // Upper nibble
//         uint8_t second_nibble_run_length = packed_run_lengths & 0x0F;         // Lower nibble

//         // Move to the next index where the actual data starts
//         ++i;

//         // Handle the first run length's data block
//         for (int run = 0; run < first_nibble_run_length; ++run) {
//             decoded_data_vec.insert(decoded_data_vec.end(),
//                                     encoded_data_vec.begin() + i,
//                                     encoded_data_vec.begin() + i + data_type_size);
//         }

//         // Move past the data block we just processed
//         i += data_type_size;

//         // Handle the second run length's data block, if there is one
//         if (second_nibble_run_length > 0 && i < encoded_data_vec.size()) {
//             for (int run = 0; run < second_nibble_run_length; ++run) {
//                 decoded_data_vec.insert(decoded_data_vec.end(),
//                                         encoded_data_vec.begin() + i,
//                                         encoded_data_vec.begin() + i + data_type_size);
//             }
//             // Move past the second data block
//             i += data_type_size;
//         }
//     }
// }


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