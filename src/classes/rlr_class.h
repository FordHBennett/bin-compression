#pragma once

#include "common_stats.h"
#include "../functions/file_functions.h"
#include <vector>
#include <filesystem>
#include <cstdint>




class RLR : public CommonStats {
    public:
        // Constructors
        RLR();
        ~RLR() = default;

        // void Read_File(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row);

        void Encode_With_One_Byte_Run_Length();
        void Decode_With_One_Byte_Run_Length();

        // void Encode_Using_Planet_Data_Headers(const int& number_of_bytes_per_row, const int& row_number);
        // void Decode_Using_Planet_Data_Headers(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row_number);

        // void Write_Compressed_File(const std::filesystem::path& file_path) const;
        // void Write_Decompressed_File(const std::filesystem::path& file_path) const;


        //getters
        const char* Get_Compression_Type() const;
        const std::vector<char>& Get_Encoded_Data_Vec() const;
        const std::vector<char>& Get_Decoded_Data_Vec() const;
        const std::vector<char>& Get_Binary_Data_Vec() const;

        //setters
        void Set_Binary_Data_Vec(const std::vector<char>& data){
            binary_data_vec = data;
        }

        uint64_t number_of_bytes_per_row = 0;
        uint64_t row_number = 0;

    private:
        // Private member variables
        char const* compression_type = "rlr";
        std::vector<char> binary_data_vec;
        std::vector<char> encoded_data_vec;
        std::vector<char> decoded_data_vec;

        // Private member functions
        template<typename T>
        void Run_Length_Encode(const std::vector<char>& data, std::vector<char>& encoded_data);

        template<typename T>
        void Run_Length_Decode(const std::vector<char>& encoded_data, std::vector<char>& decoded_data, size_t& write_index){
            for (size_t i = 0; i < (encoded_data.size()); i += sizeof(T) + 1) {
                if (i + 1 + sizeof(T) > encoded_data.size()) {
                    // PRINT_DEBUG("Error: Invalid run-length encoded data.");
                    // ERROR_MSG_AND_EXIT("Size of encoded data:" + std::to_string(encoded_data.size()) + ", i: " + std::to_string(i) + ", sizeof(T): " + std::to_string(sizeof(T)));
                    // std::cerr << "Error: Invalid run-length encoded data." << '\n';
                }

                uint8_t run_length = static_cast<uint8_t>(encoded_data[i]);
                T value;
                std::memcpy(&value, &encoded_data[i + 1], sizeof(T)); 

                std::fill_n(reinterpret_cast<T*>(&decoded_data[write_index]), run_length, value);
                write_index += run_length * sizeof(T);
            }
        }

};