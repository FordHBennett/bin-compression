#include "rlr_class.hpp"
#include <fstream>
#include <iostream>
#include <vector>

#include "../functions/debug_functions.hpp"

//Constructors
RLR::RLR(){}

RLR::RLR(const RLR& other) {}

RLR& RLR::operator=(const RLR& other) {}

RLR::~RLR() {}

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

// void RLR::Encode() {
//     // Clear the encoded data vector
//     encoded_data_vec.clear();

//     int data_type_size = this->Get_Data_Type_Size();
//     int byte_index = 0;

//     while(byte_index < binary_data_vec.size()) {
//         uint8_t run_length = 1;
//         int next_index = byte_index + data_type_size;

//         while(next_index < binary_data_vec.size()) {
//             bool blocks_match_flag = true;
//             for(int byte = 0; byte < data_type_size; byte++) {
//                 if(binary_data_vec.at(byte_index + byte) != binary_data_vec.at(next_index + byte)) {
//                     blocks_match_flag = false;
//                     break;
//                 }
//             }
//             // If blocks match and run length is less than max value for 1 byte
//             if(blocks_match_flag && (run_length < 255)) {
//                 run_length++;
//                 next_index += data_type_size;
//             } else {
//                 // Either blocks don't match or run length is at maximum, so break the loop
//                 break;
//             }
//         }

//         // Store the run length
//         encoded_data_vec.emplace_back(static_cast<char>(run_length));

//         // Store the data block corresponding to the run length
//         for(int byte = 0; byte < data_type_size; byte++) {
//             encoded_data_vec.emplace_back(binary_data_vec.at(byte_index + byte));
//         }

//         // Move to the next unique data block in binary_data_vec
//         byte_index = next_index;
//     }

//     // Optimize storage of the vector
//     encoded_data_vec.shrink_to_fit();
// }

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

void RLR::Decode_With_One_Nibble_Run_Length() {
    decoded_data_vec.clear();

    decoded_data_vec.reserve(binary_data_vec.size() * 2); // More space might be needed due to smaller run lengths

    const int data_type_size = this->Get_Data_Type_Size();
    auto it = encoded_data_vec.begin();

    while(it != encoded_data_vec.end()) {
        uint8_t packed_run_lengths = static_cast<uint8_t>(*it++);
        uint8_t first_nibble_run_length = (packed_run_lengths >> 4) & 0x0F;
        uint8_t second_nibble_run_length = packed_run_lengths & 0x0F;

        for (int run = 0; run < first_nibble_run_length; ++run) {
            decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
        }
        it += data_type_size;

        // Check if there is a second run length in this byte
        if (second_nibble_run_length > 0 && it != encoded_data_vec.end()) {
            for (int run = 0; run < second_nibble_run_length; ++run) {
                decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
            }
            it += data_type_size;
        }
    }
}


//rewrite this where the 2 run lengths are stored in the same byte using a bit mask
// This would be done by calculating 2 run lengths and then storing them in the same byte
// void RLR::Encode_With_One_Nibble_Run_Length() {
//     // Clear the encoded data vector
//     encoded_data_vec.clear();

//     const int data_type_size = this->Get_Data_Type_Size();
//     encoded_data_vec.reserve(binary_data_vec.size() / 2);
//     int byte_index = 0;
//     std::vector<char> current_block;
//     current_block.reserve(data_type_size);

//     while (byte_index < binary_data_vec.size()) {
//         uint8_t first_nibble_run_length = 1;
//         int first_nibble_next_index = byte_index + data_type_size;

//         // std::vector<char> current_block(binary_data_vec.begin() + byte_index,
//         //                                 binary_data_vec.begin() + byte_index + data_type_size);
//         current_block.assign(binary_data_vec.begin() + byte_index,
//                                         binary_data_vec.begin() + byte_index + data_type_size);

//         while (first_nibble_next_index < binary_data_vec.size() && first_nibble_run_length < 15) {
//             if (std::equal(current_block.begin(), current_block.end(),
//                            binary_data_vec.begin() + first_nibble_next_index)) {
//                 first_nibble_run_length++;
//                 first_nibble_next_index += data_type_size;
//             } else {
//                 break;
//             }
//         }

//         byte_index = first_nibble_next_index;

//         uint8_t second_nibble_run_length = 1;
//         int second_nibble_next_index = byte_index + data_type_size;

//         current_block.assign(binary_data_vec.begin() + byte_index,
//                                         binary_data_vec.begin() + byte_index + data_type_size);

//         while (second_nibble_next_index < binary_data_vec.size() && second_nibble_run_length < 15) {
//             if (std::equal(current_block.begin(), current_block.end(),
//                            binary_data_vec.begin() + second_nibble_next_index)) {
//                 second_nibble_run_length++;
//                 second_nibble_next_index += data_type_size;
//             } else {
//                 break;
//             }
//         }

//         encoded_data_vec.push_back(static_cast<char>((first_nibble_run_length << 4) | second_nibble_run_length));
//         encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

//     }

//     encoded_data_vec.shrink_to_fit();
// }

// void RLR::Decode_With_One_Nibble_Run_Length() {
//     // Clear the vector to make sure we're starting fresh.
//     decoded_data_vec.clear();

//     decoded_data_vec.reserve(binary_data_vec.size());

//     const int data_type_size = this->Get_Data_Type_Size();
//     auto it = encoded_data_vec.begin();

//     while(it != encoded_data_vec.end()) {
//         uint8_t run_length = static_cast<uint8_t>(*it++);

//         //0x0F is 00001111 in binary
//         uint8_t first_nibble_run_length = run_length & 0x0F;
//         // 0xF0 is 1111000 in binary
//         uint8_t second_nibble_run_length = run_length & 0xF0;

//         for (int run = 0; run < first_nibble_run_length; ++run) {
//             decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
//         }

//         it += data_type_size;

//         for (int run = 0; run < second_nibble_run_length; ++run) {
//             decoded_data_vec.insert(decoded_data_vec.end(), it, it + data_type_size);
//         }

//         it += data_type_size;
//     }
// }



// void RLR::Decode() {
//     // // clear the decoded data vector
//     // decoded_data_vec.clear();



//     // // Initialize byte_index to 0 to start from the beginning of the binary_data_vec
//     // int byte_index = 0;

//     // // Use a while loop instead of a for loop to manually control byte_index increments
//     // while(byte_index < encoded_data_vec.size()){
//     //     uint8_t run_length = static_cast<uint8_t>(encoded_data_vec.at(byte_index));
//     //     for(int i = 0; i < run_length; i++){
//     //         for(int byte = 1; byte < this->Get_Data_Type_Size(); byte++){
//     //             decoded_data_vec.emplace_back(encoded_data_vec.at(byte_index+byte));
//     //         }
//     //     }
//     //     byte_index += this->Get_Data_Type_Size();
//     // }

//     // // Optimize storage of the vector
//     // decoded_data_vec.shrink_to_fit();
//        // Clear the decoded data vector
//     decoded_data_vec.clear();

//     // Data type size(for example, for a byte, this would be 1)
//     int data_type_size = this->Get_Data_Type_Size();

//     // Initialize byte_index to 0 to start from the beginning of the encoded_data_vec
//     int byte_index = 0;

//     // Decode the data until we reach the end of the encoded data
//     while(byte_index < encoded_data_vec.size()) {
//         // Get the run length which is the first byte of the sequence
//         uint8_t run_length = static_cast<uint8_t>(encoded_data_vec.at(byte_index));

//         // Iterate for the number of run lengths
//         for(int i = 0; i < run_length; i++) {
//             // Copy the data item following the run length byte
//             for(int j = 0; j < data_type_size; j++) {
//                 if(byte_index + 1 + j < encoded_data_vec.size()) { // Ensure no overflow
//                     decoded_data_vec.emplace_back(encoded_data_vec.at(byte_index + 1 + j));
//                 }
//             }
//         }

//         // Move to the next sequence by skipping the current run length and data item
//         byte_index += 1 + data_type_size;
//     }

//     // Optimize storage of the vector
//     decoded_data_vec.shrink_to_fit();

// }

// void RLR::Write_Compressed_File(const std::filesystem::path& file_path, const int& row) const {
//     // std::filesystem::path compressed_path = file_path.parent_path() / std::string{"compressed_decompressed_files"} / file_path.filename().stem() / file_path.filename().stem().string() + std::string{"_row_"} + std::to_string(row) + file_path.extension().string();
//     std::filesystem::path compressed_path =
//     file_path.parent_path() /
//     "compressed_decompressed_rlr_files" /
//     file_path.stem().string() / "compressed_files"/ (std::string{"row_"} + std::to_string(row) + file_path.extension().string());

//     // Make sure the directory exists before trying to use the file path
//     if(!std::filesystem::exists(compressed_path.parent_path())){
//         std::filesystem::create_directories(compressed_path.parent_path());
//     }
//     std::ofstream encoded_output_file(compressed_path, std::ios::binary);
//     if(!encoded_output_file) {
//         ERROR_MSG_AND_EXIT("Error: Unable to create the run-length encoded file.");
//     }

//     // Writing encoded data to file
//     encoded_output_file.write(encoded_data_vec.data(), encoded_data_vec.size());

//     encoded_output_file.close();
// }

// void RLR::Write_Decompressed_File(const std::filesystem::path& file_path, const int& row) const {
//     // std::filesystem::path compressed_path = file_path.parent_path() /std::string{"compressed_decompressed_files"}/ file_path.filename();
//     std::filesystem::path compressed_path =
//     file_path.parent_path() /
//     "compressed_decompressed_rlr_files" /
//     file_path.stem().string() / "decompressed_files" / (std::string{"row_"} + std::to_string(row) + file_path.extension().string());

//     // Make sure the directory exists before trying to use the file path
//     if(!std::filesystem::exists(compressed_path.parent_path())){
//         std::filesystem::create_directories(compressed_path.parent_path());
//     }
//     std::ofstream decoded_output_file(compressed_path, std::ios::binary);
//     if(!decoded_output_file) {
//         ERROR_MSG_AND_EXIT("Error: Unable to create the run-length decoded file.");
//     }

//     // Writing decoded data to file
//     decoded_output_file.write(decoded_data_vec.data(), decoded_data_vec.size());

//     decoded_output_file.close();
// }

void RLR::Write_Compressed_File(const std::filesystem::path& file_path) const {
    try {
        std::filesystem::path compressed_path = file_path.parent_path() / "compressed_decompressed_rlr_files" / file_path.stem() / file_path.filename();

        // Make sure the directory exists before trying to use the file path
        std::filesystem::create_directories(compressed_path.parent_path());

        // Open the file in binary and append mode
        std::ofstream encoded_output_file(compressed_path, std::ios::binary | std::ios::app);

        // Writing encoded data to the end of file
        encoded_output_file.write(encoded_data_vec.data(), encoded_data_vec.size());

        encoded_output_file.close();
    } catch (const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error: Unable to create the run-length encoded file.\n"} + std::string{"ERROR CODE: "} + std::string{e.what()});
    }
}

void RLR::Write_Decompressed_File(const std::filesystem::path& file_path) const {
    try{
        std::filesystem::path compressed_path = file_path.parent_path() / "compressed_decompressed_rlr_files" / file_path.stem() / file_path.filename();

        // Make sure the directory exists before trying to use the file path
        std::filesystem::create_directories(compressed_path.parent_path());

        // Open the file in binary and append mode
        std::ofstream decoded_output_file(compressed_path, std::ios::binary | std::ios::app);

        // Writing decoded data to the end of file
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