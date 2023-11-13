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
    input_file.close();
}

//FASTER
void RLR::Encode_With_One_Nibble_Run_Length() {
    encoded_data_vec.clear();
    encoded_data_vec.reserve(binary_data_vec.size());

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;
    std::vector<char> first_nibble_run_block(data_type_size, 0);
    std::vector<char> second_nibble_run_block(data_type_size, 0);

    while (byte_index < binary_data_vec.size()) {
        uint8_t first_nibble_run_length = 1;
        uint8_t second_nibble_run_length = 0;
        auto next_index = byte_index + data_type_size;

        first_nibble_run_block.assign(binary_data_vec.begin() + byte_index, binary_data_vec.begin() + byte_index + data_type_size);

        while ((next_index < binary_data_vec.size()) && (first_nibble_run_length < ONE_NIBBLE_MAX) &&
                (std::equal(first_nibble_run_block.begin(), first_nibble_run_block.end(), binary_data_vec.begin() + next_index))) {
            first_nibble_run_length++;
            next_index += data_type_size;
        }

        byte_index = next_index;

        if (byte_index < binary_data_vec.size()) {
            second_nibble_run_block.assign(binary_data_vec.begin() + byte_index, binary_data_vec.begin() + byte_index + data_type_size);

            while ((next_index < binary_data_vec.size()) && (second_nibble_run_length < ONE_NIBBLE_MAX) &&
                    (std::equal(second_nibble_run_block.begin(), second_nibble_run_block.end(), binary_data_vec.begin() + next_index))) {
                second_nibble_run_length++;
                next_index += data_type_size;
            }
        }

        byte_index = next_index;

        encoded_data_vec.emplace_back(static_cast<char>((first_nibble_run_length << 4) | second_nibble_run_length));
        encoded_data_vec.insert(encoded_data_vec.end(), first_nibble_run_block.begin(), first_nibble_run_block.end());

        if (second_nibble_run_length) {
            encoded_data_vec.insert(encoded_data_vec.end(), second_nibble_run_block.begin(), second_nibble_run_block.end());
        }
    }

    encoded_data_vec.shrink_to_fit();
}


//FASTER
void RLR::Decode_With_One_Nibble_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        const uint8_t packed_run_lengths = static_cast<uint8_t>(encoded_data_vec[byte_index]);
        const uint8_t first_nibble_run_length = packed_run_lengths >> 4;
        const uint8_t second_nibble_run_length = packed_run_lengths & 0x0F;

        byte_index++;

        decoded_data_vec.insert(decoded_data_vec.end(), first_nibble_run_length * data_type_size,encoded_data_vec[byte_index]);
        byte_index += data_type_size;


        if ((second_nibble_run_length > 0) && (byte_index + data_type_size <= encoded_data_vec.size())) {
            decoded_data_vec.insert(decoded_data_vec.end(), second_nibble_run_length * data_type_size, encoded_data_vec[byte_index]);
            byte_index += data_type_size;
        }
    }
}


void RLR::Encode_With_One_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    auto byte_index = 0;

    std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                    binary_data_vec.begin() + byte_index + data_type_size);

    while (byte_index < binary_data_vec.size()) {
        uint8_t run_length = 1;
        int next_index = byte_index + data_type_size;
        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while (next_index < binary_data_vec.size() && (run_length < ONE_BYTE_MAX) &&
                (std::equal(current_block.begin(), current_block.end(), binary_data_vec.begin() + next_index))) {

            run_length++;
            next_index += data_type_size;
        }
        encoded_data_vec.push_back(static_cast<char>(run_length));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());
        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_One_Byte_Run_Length() {
    decoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        const uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]);

        byte_index++;

        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);

        byte_index += data_type_size;

    }
}


void RLR::Encode_With_Two_Byte_Run_Length() {
    encoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    auto byte_index = 0;

    std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

    while (byte_index < binary_data_vec.size()) {
        auto run_length = 1;
        auto next_index = byte_index + data_type_size;
        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while ((next_index < binary_data_vec.size()) && (run_length <= TWO_BYTE_MAX) &&
                (std::equal(current_block.begin(), current_block.end(), binary_data_vec.begin() + next_index))) {
                run_length++;
                next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Two_Byte_Run_Length() {
    decoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        uint16_t run_length = static_cast<uint16_t>(encoded_data_vec[byte_index]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) & 0xFF;

        byte_index += 2;

        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }
}

void RLR::Encode_With_Three_Byte_Run_Length(){
    encoded_data_vec.clear();
    const int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    auto byte_index = 0;

    std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

    while (byte_index < binary_data_vec.size()) {
        auto run_length = 1;
        auto next_index = byte_index + data_type_size;
        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while ((next_index < binary_data_vec.size()) && (run_length <= THREE_BYTE_MAX) &&
                (std::equal(current_block.begin(), current_block.end(), binary_data_vec.begin() + next_index))) {
                run_length++;
                next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 16));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Three_Byte_Run_Length(){
    decoded_data_vec.clear();
    // decoded_data_vec.reserve(binary_data_vec.size());

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        uint32_t run_length = static_cast<uint32_t>(encoded_data_vec[byte_index]) << 16;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 2]) & 0xFF;

        byte_index += 3;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    decoded_data_vec.shrink_to_fit();
}

void RLR::Encode_With_Four_Byte_Run_Length(){
    encoded_data_vec.clear();
    const int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    auto byte_index = 0;

    std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

    while (byte_index < binary_data_vec.size()) {
        uint32_t run_length = 1;
        auto next_index = byte_index + data_type_size;
        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while ((next_index < binary_data_vec.size()) && (run_length <= FOUR_BYTE_MAX) &&
                (std::equal(current_block.begin(), current_block.end(), binary_data_vec.begin() + next_index))) {
                run_length++;
                next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 24));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 16));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Four_Byte_Run_Length(){
    decoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        uint32_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]) << 24;
        run_length |= static_cast<uint32_t>(encoded_data_vec[byte_index + 1]) << 16;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 2]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 3]) & 0xFF;

        byte_index += 4;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    decoded_data_vec.shrink_to_fit();

}

void RLR::Encode_With_Five_Byte_Run_Length(){
    encoded_data_vec.clear();
    const int data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.reserve(binary_data_vec.size());
    auto byte_index = 0;
    std::vector<char> current_block(binary_data_vec.begin() + byte_index,
                                        binary_data_vec.begin() + byte_index + data_type_size);

    while (byte_index < binary_data_vec.size()) {
        boost::multiprecision::uint128_t run_length = 1;
        auto next_index = byte_index + data_type_size;
        current_block.assign(binary_data_vec.begin() + byte_index,
                             binary_data_vec.begin() + byte_index + data_type_size);

        while ((next_index < binary_data_vec.size()) && (run_length <= FIVE_BYTE_MAX) &&
                (std::equal(current_block.begin(), current_block.end(), binary_data_vec.begin() + next_index))) {
                run_length++;
                next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 32));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 24));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 16));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<uint8_t>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());

        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Five_Byte_Run_Length(){
    decoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    auto byte_index = 0;

    while (byte_index < encoded_data_vec.size()) {
        boost::multiprecision::uint128_t run_length = static_cast<boost::multiprecision::uint128_t>(encoded_data_vec[byte_index]) << 32;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) << 24;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 2]) << 16;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 3]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 4]) & 0xFF;

        byte_index += 5;
        if(!(run_length > UINT64_MAX)){
            decoded_data_vec.insert(decoded_data_vec.end(),
                                static_cast<uint64_t>(run_length) * data_type_size,
                                encoded_data_vec[byte_index]);
        } else {
            uint64_t first_64_bits = static_cast<uint64_t>(run_length >> 64);
            uint64_t last_64_bits = static_cast<uint64_t>(run_length & 0xFFFFFFFFFFFFFFFF);
            decoded_data_vec.insert(decoded_data_vec.end(),
                                first_64_bits * data_type_size,
                                encoded_data_vec[byte_index]);

            decoded_data_vec.insert(decoded_data_vec.end(),
                                last_64_bits * data_type_size,
                                encoded_data_vec[byte_index]);
        }
        byte_index += data_type_size;
    }

    decoded_data_vec.shrink_to_fit();
}

// This is too memory intensive
// void RLR::Encode_With_Burrow_Wheeler_Transformation_Little_Endian() {
//     // Clear the encoded data vector
//     encoded_data_vec.clear();

//     std::vector<bool> data_type_sized_bytes_frequencey_vec(1ULL << (this->Get_Data_Type_Size() << 3), false);

//     std::vector<char> data_type_sized_bytes_vec(this->Get_Data_Type_Size(), 0);
//     std::vector<std::vector<char>> vec_of_data_type_sized_vec(binary_data_vec.size() + this->Get_Data_Type_Size());

//     uint64_t data_type_sized_bytes_frequencey_vec_index = 0;
//     for(int byte_index = 0; byte_index < binary_data_vec.size(); byte_index += this->Get_Data_Type_Size()){
//         data_type_sized_bytes_vec.assign(binary_data_vec.begin() + byte_index, binary_data_vec.begin() + byte_index + this->Get_Data_Type_Size());
//         for(int i = 0; i < data_type_sized_bytes_vec.size(); i++){
//             data_type_sized_bytes_frequencey_vec_index |= (static_cast<uint64_t>(static_cast<uint8_t>(data_type_sized_bytes_vec[i])) << (8 * i));
//         }
//         data_type_sized_bytes_frequencey_vec[data_type_sized_bytes_frequencey_vec_index] = true;
//         vec_of_data_type_sized_vec[byte_index / this->Get_Data_Type_Size()] = data_type_sized_bytes_vec;
//     }

//     bool sentinel_found = false;
//     sentinel_vec.reserve(this->Get_Data_Type_Size());
//     for(uint64_t i = 0; i < data_type_sized_bytes_frequencey_vec.size(); ++i) {
//         if(!data_type_sized_bytes_frequencey_vec[i]) {
//             sentinel_found = true;
//             for(int j = 0; j < sentinel_vec.size(); ++j) {
//                 sentinel_vec[j] = static_cast<char>((i >> (8 * j)) & 0xFF);
//                 PRINT_DEBUG(std::string{"sentinel_vec["} + std::to_string(j) + std::string{"]: "} + std::to_string(sentinel_vec[j]));
//             }
//             break;
//         }
//     }

//     if(!sentinel_found) {
//         ERROR_MSG("Error: Unable to find a sentinel.");
//         return;
//     }

//     vec_of_data_type_sized_vec.push_back(sentinel_vec);

//     std::vector<std::vector<std::vector<char>>> rotations_vec;
//     rotations_vec.reserve(vec_of_data_type_sized_vec.size());
//     rotations_vec.push_back(vec_of_data_type_sized_vec);

//     for(size_t rotation = 1; rotation < vec_of_data_type_sized_vec.size(); rotation++) {
//         rotations_vec.push_back(std::vector<std::vector<char>>(vec_of_data_type_sized_vec.size()));
//         // rotations_vec[rotation].insert(rotations_vec[rotation].end(), rotations_vec[rotation-1].begin() + 1, rotations_vec[rotation-1].end());
//         // rotations_vec[rotation].insert(rotations_vec[rotation].end(), rotations_vec[rotation-1].begin(), rotations_vec[rotation-1].begin() + 1);
//         // rotate rotations_vec[rotation] by 1 to the right and append to rotations_vec[rotation]
//     }
//     // Sort the rotations_vec
//     std::sort(rotations_vec[0].begin(), rotations_vec[0].end());

//     // Extract the last column from the sorted rotations_vec
//     // for(size_t i = 0; i < rotations_vec.size(); i++) {
//     //     for(size_t j = 0; j < rotations_vec[i][rotations_vec[i][0].size()].size(); j++) {
//     //         encoded_data_vec.push_back(rotations_vec[i][rotations_vec[i].size()-1][j]);
//     //         PRINT_DEBUG(std::string{"encoded_data_vec["} + std::to_string(encoded_data_vec.size()-1) + std::string{"]: "} + std::to_string(encoded_data_vec[encoded_data_vec.size()-1]));
//     //     }
//     // }

//     // Extract the last column from each row of sorted rotations_vec
//     for(size_t i = 0; i < rotations_vec.size(); i++) {
//         for(size_t j = 0; j < rotations_vec[i][rotations_vec[i][0].size()].size(); j++) {
//             encoded_data_vec.push_back(rotations_vec[i][rotations_vec[i].size()-1][j]);
//             PRINT_DEBUG(std::string{"encoded_data_vec["} + std::to_string(encoded_data_vec.size()-1) + std::string{"]: "} + std::to_string(encoded_data_vec[encoded_data_vec.size()-1]));
//         }
//     }


//     encoded_data_vec.shrink_to_fit();


// }

// void RLR::Decode_With_Inverse_Burrow_Wheeler_Transformation_Little_Endian(){
//     // First, we'll recreate the table of rotations.
//     std::vector<std::vector<char>> table(encoded_data_vec.size() / this->Get_Data_Type_Size(), std::vector<char>(this->Get_Data_Type_Size()));

//     // Fill the table with the data from the BWT vector.
//     for (size_t i = 0; i < encoded_data_vec.size(); i++) {
//         table[i / this->Get_Data_Type_Size()][i % this->Get_Data_Type_Size()] = encoded_data_vec[i];
//     }

//     // Sort the table by each column, adding characters from the BWT in each iteration.
//     for (size_t j = 0; j < this->Get_Data_Type_Size() - 1; j++) {
//         // Insert BWT chars into the table.
//         for (size_t i = 0; i < table.size(); i++) {
//             table[i].insert(table[i].begin(), encoded_data_vec[i * this->Get_Data_Type_Size() + j % this->Get_Data_Type_Size()]);
//         }
//         // Sort the table.
//         std::sort(table.begin(), table.end());
//     }

//     // Find the row that ends with the sentinel. This is the original data.
//     decoded_data_vec.clear();
//     for (const auto& row : table) {
//         if (std::equal(row.end() - sentinel_vec.size(), row.end(), sentinel_vec.begin())) {
//             decoded_data_vec.assign(row.begin(), row.end() - sentinel_vec.size());
//             break;
//         }
//     }

//     if (decoded_data_vec.empty()) {
//         ERROR_MSG_AND_EXIT("Error: Unable to find the sentinel in the table of rotations.");
//     }

//     // Remove the sentinel from the end of the binary data vector.
//     decoded_data_vec.erase(decoded_data_vec.end() - this->Get_Data_Type_Size(), decoded_data_vec.end());
// }

void RLR::Encode_With_Move_To_Front_Transformation() {
    encoded_data_vec.clear();

    const uint8_t data_type_size = this->Get_Data_Type_Size();
    const auto alphabet_size = 1ULL << (data_type_size * 8);
    std::vector<char> alphabet_vec = [&] {
        switch (data_type_size) {
            case 1:
                return  std::vector<char>(ONE_BYTE_ALPHABET_VEC);
            case 2:
                return std::vector<char>(TWO_BYTE_ALPHABET_VEC);
            default:
                ERROR_MSG_AND_EXIT("Error: Data type size is not 1");
        }
    }();

    // Move-to-front encoding
    for(auto byte_index = 0; byte_index < binary_data_vec.size(); byte_index += data_type_size) {
        for(auto i = 0; i < alphabet_size; i++) {
            if(std::equal(binary_data_vec.begin() + byte_index,
                          binary_data_vec.begin() + byte_index + data_type_size,
                          alphabet_vec.begin() + i * data_type_size)) {
                encoded_data_vec.push_back(i);
                // Move to front
                std::rotate(alphabet_vec.begin(), alphabet_vec.begin() + i * data_type_size, alphabet_vec.begin() + (i + 1) * data_type_size);
                break;
            }
        }
    }

    encoded_data_vec.shrink_to_fit();
}


void RLR::Decode_With_Move_To_Front_Transformation() {
    decoded_data_vec.clear();

    uint8_t data_type_size = this->Get_Data_Type_Size();
    std::vector<char> alphabet_vec = [&] {
        switch (data_type_size) {
            case 1:
                return  std::vector<char>(ONE_BYTE_ALPHABET_VEC);
            case 2:
                return std::vector<char>(TWO_BYTE_ALPHABET_VEC);
            default:
                ERROR_MSG_AND_EXIT("Error: Data type size is not 1 or 2.");
                return std::vector<char>();
        }
    }();

    for(auto index : encoded_data_vec) {
        decoded_data_vec.resize(decoded_data_vec.size() + data_type_size);
        memcpy(decoded_data_vec.data() + decoded_data_vec.size() - data_type_size, alphabet_vec.data() + index * data_type_size, data_type_size);
        // Move to front
        std::rotate(alphabet_vec.begin(), alphabet_vec.begin() + index * data_type_size, alphabet_vec.begin() + (index + 1) * data_type_size);
    }

    decoded_data_vec.shrink_to_fit();
}


void RLR::Encode_With_Delta_Transformation(){
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    encoded_data_vec.insert(encoded_data_vec.end(), binary_data_vec.begin(), binary_data_vec.begin() + data_type_size);

    std::vector<char> current_block(binary_data_vec.begin(), binary_data_vec.begin() + data_type_size);
    std::vector<char> previous_block(data_type_size, 0);

    std::vector<char> delta_block(data_type_size, 0);
    std::transform(current_block.begin(), current_block.end(), previous_block.begin(), delta_block.begin(), std::minus<char>());
    encoded_data_vec.insert(encoded_data_vec.end(), delta_block.begin(), delta_block.end());

    for(auto byte_index = data_type_size; byte_index < binary_data_vec.size(); byte_index += data_type_size) {
        // memcpy(previous_block.data(), current_block.data(), data_type_size);
        // memcpy(current_block.data(), binary_data_vec.data() + byte_index, data_type_size);
        std::copy(current_block.begin(), current_block.end(), previous_block.begin());
        std::copy(binary_data_vec.begin() + byte_index, binary_data_vec.begin() + byte_index + data_type_size, current_block.begin());

        std::transform(current_block.begin(), current_block.end(), previous_block.begin(), delta_block.begin(), std::minus<char>());
        encoded_data_vec.insert(encoded_data_vec.end(), delta_block.begin(), delta_block.end());
    }

   encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_Delta_Transformation(){
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();
    decoded_data_vec.insert(decoded_data_vec.end(), encoded_data_vec.begin(), encoded_data_vec.begin() + data_type_size);

    std::vector<char> current_block(encoded_data_vec.begin(), encoded_data_vec.begin() + data_type_size);
    std::vector<char> previous_block(data_type_size, 0);

    std::vector<char> delta_block(data_type_size, 0);
    for(auto byte_index = data_type_size; byte_index < encoded_data_vec.size(); byte_index += data_type_size) {
        // memcpy(previous_block.data(), current_block.data(), data_type_size);
        // memcpy(delta_block.data(), encoded_data_vec.data() + byte_index, data_type_size);
        std::copy(current_block.begin(), current_block.end(), previous_block.begin());
        std::copy(encoded_data_vec.begin() + byte_index, encoded_data_vec.begin() + byte_index + data_type_size, delta_block.begin());

        std::transform(delta_block.begin(), delta_block.end(), previous_block.begin(), current_block.begin(), std::plus<char>());
        decoded_data_vec.insert(decoded_data_vec.end(), current_block.begin(), current_block.end());
    }

    decoded_data_vec.shrink_to_fit();
}

void RLR::Encode_With_XOR_Transformation() {
    encoded_data_vec.clear();
    encoded_data_vec.resize(binary_data_vec.size(), 0);

    encoded_data_vec[0] = binary_data_vec[0];

    for(auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
        encoded_data_vec[byte_index] = static_cast<char>(encoded_data_vec[byte_index - 1] ^ binary_data_vec[byte_index]);
    }
}

void RLR::Decode_With_XOR_Transformation() {
    decoded_data_vec.clear();
    decoded_data_vec.resize(encoded_data_vec.size(), 0);

    decoded_data_vec[0] = encoded_data_vec[0];

    for(auto byte_index = 1; byte_index < encoded_data_vec.size(); byte_index++) {
        decoded_data_vec[byte_index] = static_cast<char>(encoded_data_vec[byte_index - 1] ^ encoded_data_vec[byte_index]);
    }
}

// void RLR::Encode_With_XOR_Transformation_With_One_Byte_Run_Length() {
//     std::vector<char> xor_encoded_data;
//     xor_encoded_data.resize(binary_data_vec.size(), 0);

//     xor_encoded_data[0] = binary_data_vec[0];

//     for(auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
//         xor_encoded_data[byte_index] = static_cast<char>(xor_encoded_data[byte_index - 1] ^ binary_data_vec[byte_index]);
//     }

//     // run-length encoding
//     encoded_data_vec.clear();
//     const uint8_t data_type_size = this->Get_Data_Type_Size();
//     encoded_data_vec.reserve(xor_encoded_data.size());
//     auto byte_index = 0;

//     std::vector<char> current_block(xor_encoded_data.begin() + byte_index,
//                                     xor_encoded_data.begin() + byte_index + data_type_size);

//     while (byte_index < xor_encoded_data.size()) {
//         uint8_t run_length = 1;
//         int next_index = byte_index + data_type_size;
//         current_block.assign(xor_encoded_data.begin() + byte_index,
//                              xor_encoded_data.begin() + byte_index + data_type_size);

//         while (next_index < xor_encoded_data.size() && (run_length < ONE_BYTE_MAX) &&
//                 (std::equal(current_block.begin(), current_block.end(), xor_encoded_data.begin() + next_index))) {

//             run_length++;
//             next_index += data_type_size;
//         }
//         encoded_data_vec.push_back(static_cast<char>(run_length));
//         encoded_data_vec.insert(encoded_data_vec.end(), current_block.begin(), current_block.end());
//         byte_index = next_index;
//     }

//     encoded_data_vec.shrink_to_fit();
// }

// void RLR::Decode_With_XOR_Transformation_With_One_Byte_Run_Length() {
//     std::vector<char> rlr_decoded;

//     // Run-Length Decoding
//     const uint8_t data_type_size = this->Get_Data_Type_Size();
//     auto byte_index = 0;

//     while (byte_index < encoded_data_vec.size()) {
//         const uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]);
//         byte_index++;
//         rlr_decoded.insert(rlr_decoded.end(),
//                             run_length * data_type_size,
//                             encoded_data_vec[byte_index]);
//         byte_index += data_type_size;
//     }

//     // XOR Transformation
//     decoded_data_vec.clear();
//     decoded_data_vec.resize(rlr_decoded.size(), 0);
//     decoded_data_vec[0] = rlr_decoded[0];

//     for (auto byte_index = 1; byte_index < rlr_decoded.size(); byte_index++) {
//         decoded_data_vec[byte_index] = static_cast<char>(rlr_decoded[byte_index - 1] ^ rlr_decoded[byte_index]);
//     }
// }

void RLR::Encode_With_XOR_Transformation_With_One_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (binary_data_vec.empty()) {
        return;
    }

    // Perform XOR transformation in-place
    for (auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
        binary_data_vec[byte_index] ^= binary_data_vec[byte_index - 1];
    }

    // Run-length encoding
    auto byte_index = 0;
    while (byte_index < binary_data_vec.size()) {
        uint8_t run_length = 1;
        int next_index = byte_index + data_type_size;

        while (next_index < binary_data_vec.size() && run_length < ONE_BYTE_MAX &&
               std::equal(binary_data_vec.begin() + byte_index,
                          binary_data_vec.begin() + byte_index + data_type_size,
                          binary_data_vec.begin() + next_index)) {
            run_length++;
            next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<char>(run_length));
        encoded_data_vec.insert(encoded_data_vec.end(),
                                binary_data_vec.begin() + byte_index,
                                binary_data_vec.begin() + byte_index + data_type_size);
        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_XOR_Transformation_With_One_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (encoded_data_vec.empty()) {
        return;
    }

    // Run-length decoding
    auto byte_index = 0;
    while (byte_index < encoded_data_vec.size()) {
        const uint8_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]);
        byte_index++;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    // XOR Transformation
    for (auto i = decoded_data_vec.size() - 1; i > 0; i--) {
        decoded_data_vec[i] ^= decoded_data_vec[i - 1];
    }
}

void RLR::Encode_With_XOR_Transformation_With_Two_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (binary_data_vec.empty()) {
        return;
    }

    // Perform XOR transformation in-place
    for (auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
        binary_data_vec[byte_index] ^= binary_data_vec[byte_index - 1];
    }

    // Run-length encoding
    auto byte_index = 0;
    while (byte_index < binary_data_vec.size()) {
        uint16_t run_length = 1;
        int next_index = byte_index + data_type_size;

        while (next_index < binary_data_vec.size() && run_length < TWO_BYTE_MAX &&
               std::equal(binary_data_vec.begin() + byte_index,
                          binary_data_vec.begin() + byte_index + data_type_size,
                          binary_data_vec.begin() + next_index)) {
            run_length++;
            next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<char>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(),
                                binary_data_vec.begin() + byte_index,
                                binary_data_vec.begin() + byte_index + data_type_size);
        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_XOR_Transformation_With_Two_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (encoded_data_vec.empty()) {
        return;
    }

    // Run-length decoding
    auto byte_index = 0;
    while (byte_index < encoded_data_vec.size()) {
        uint16_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) & 0xFF;

        byte_index += 2;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    // XOR Transformation
    for (auto i = decoded_data_vec.size() - 1; i > 0; i--) {
        decoded_data_vec[i] ^= decoded_data_vec[i - 1];
    }
}

void RLR::Encode_With_XOR_Transformation_With_Three_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (binary_data_vec.empty()) {
        return;
    }

    // Perform XOR transformation in-place
    for (auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
        binary_data_vec[byte_index] ^= binary_data_vec[byte_index - 1];
    }

    // Run-length encoding
    auto byte_index = 0;
    while (byte_index < binary_data_vec.size()) {
        uint32_t run_length = 1;
        int next_index = byte_index + data_type_size;

        while (next_index < binary_data_vec.size() && run_length < THREE_BYTE_MAX &&
               std::equal(binary_data_vec.begin() + byte_index,
                          binary_data_vec.begin() + byte_index + data_type_size,
                          binary_data_vec.begin() + next_index)) {
            run_length++;
            next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<char>(run_length >> 16));
        encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<char>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(),
                                binary_data_vec.begin() + byte_index,
                                binary_data_vec.begin() + byte_index + data_type_size);
        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_XOR_Transformation_With_Three_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (encoded_data_vec.empty()) {
        return;
    }

    // Run-length decoding
    auto byte_index = 0;
    while (byte_index < encoded_data_vec.size()) {
        uint32_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]) << 16;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 2]) & 0xFF;

        byte_index += 3;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    // XOR Transformation
    for (auto i = decoded_data_vec.size() - 1; i > 0; i--) {
        decoded_data_vec[i] ^= decoded_data_vec[i - 1];
    }
}

void RLR::Encode_With_XOR_Transformation_With_Four_Byte_Run_Length() {
    encoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (binary_data_vec.empty()) {
        return;
    }

    // Perform XOR transformation in-place
    for (auto byte_index = 1; byte_index < binary_data_vec.size(); byte_index++) {
        binary_data_vec[byte_index] ^= binary_data_vec[byte_index - 1];
    }

    // Run-length encoding
    auto byte_index = 0;
    while (byte_index < binary_data_vec.size()) {
        uint32_t run_length = 1;
        int next_index = byte_index + data_type_size;

        while (next_index < binary_data_vec.size() && run_length < FOUR_BYTE_MAX &&
               std::equal(binary_data_vec.begin() + byte_index,
                          binary_data_vec.begin() + byte_index + data_type_size,
                          binary_data_vec.begin() + next_index)) {
            run_length++;
            next_index += data_type_size;
        }

        encoded_data_vec.push_back(static_cast<char>(run_length >> 24));
        encoded_data_vec.push_back(static_cast<char>(run_length >> 16));
        encoded_data_vec.push_back(static_cast<char>(run_length >> 8));
        encoded_data_vec.push_back(static_cast<char>(run_length & 0xFF));
        encoded_data_vec.insert(encoded_data_vec.end(),
                                binary_data_vec.begin() + byte_index,
                                binary_data_vec.begin() + byte_index + data_type_size);
        byte_index = next_index;
    }

    encoded_data_vec.shrink_to_fit();
}

void RLR::Decode_With_XOR_Transformation_With_Four_Byte_Run_Length() {
    decoded_data_vec.clear();
    const uint8_t data_type_size = this->Get_Data_Type_Size();

    if (encoded_data_vec.empty()) {
        return;
    }

    // Run-length decoding
    auto byte_index = 0;
    while (byte_index < encoded_data_vec.size()) {
        uint32_t run_length = static_cast<uint8_t>(encoded_data_vec[byte_index]) << 24;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 1]) << 16;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 2]) << 8;
        run_length |= static_cast<uint8_t>(encoded_data_vec[byte_index + 3]) & 0xFF;

        byte_index += 4;
        decoded_data_vec.insert(decoded_data_vec.end(),
                                run_length * data_type_size,
                                encoded_data_vec[byte_index]);
        byte_index += data_type_size;
    }

    // XOR Transformation
    for (auto i = decoded_data_vec.size() - 1; i > 0; i--) {
        decoded_data_vec[i] ^= decoded_data_vec[i - 1];
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