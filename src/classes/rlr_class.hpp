#pragma once

#include "common_stats.hpp"
#include "../functions/file_functions.hpp"
#include <vector>
#include <filesystem>
#include <cstdint>



class RLR : public CommonStats {
    public:
        // Constructors
        RLR();
        ~RLR() = default;

        void Read_File(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row);

        void Encode_With_One_Nibble_Run_Length();
        void Decode_With_One_Nibble_Run_Length();

        void Encode_With_One_Byte_Run_Length();
        void Decode_With_One_Byte_Run_Length();
;

        void Encode_With_Move_To_Front_Transformation();
        void Decode_With_Move_To_Front_Transformation();

        void Encode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length();
        void Decode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length();


        void Encode_With_Delta_Transformation();
        void Decode_With_Delta_Transformation();

        void Encode_With_XOR_Transformation();
        void Decode_With_XOR_Transformation();

        void Encode_With_XOR_Transformation_With_One_Byte_Run_Length();
        void Decode_With_XOR_Transformation_With_One_Byte_Run_Length();

        void Encode_Using_Planet_Data_Headers(const int& number_of_bytes_per_row, const int& row_number);
        void Decode_Using_Planet_Data_Headers(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row_number);

        void Write_Compressed_File(const std::filesystem::path& file_path) const;
        void Write_Decompressed_File(const std::filesystem::path& file_path) const;


        //getters
        const char* Get_Compression_Type() const;
        const std::vector<char>& Get_Encoded_Data_Vec() const;
        const std::vector<char>& Get_Decoded_Data_Vec() const;
        const std::vector<char>& Get_Binary_Data_Vec() const;

        uint64_t number_of_bytes_per_row = 0;
        uint64_t row_number = 0;

    private:
        // Private member variables
        char const* compression_type = "rlr_1B";
        std::vector<char> binary_data_vec;
        std::vector<char> encoded_data_vec;
        std::vector<char> decoded_data_vec;

        // Private member functions
        template<typename T>
        void Run_Length_Encode(const std::vector<char>& data, std::vector<char>& encoded_data);

        template<typename T>
        void Run_Length_Decode(const std::vector<char>& encoded_data, std::vector<char>& decoded_data, size_t& write_index);

        template<typename T>
        std::vector<T> MTF_Encode_1Byte(const std::vector<T>& data);

        template<typename T>
        std::vector<T> MTF_Decode_1Byte(const std::vector<T>& data);

        template<typename T>
        std::vector<T> MTF_Encode_2Byte(const std::vector<T>& data);

        template<typename T>
        std::vector<T> MTF_Decode_2Byte(const std::vector<T>& data);


};