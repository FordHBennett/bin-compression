#pragma once

#include "common_stats.hpp"
#include "../functions/file_functions.hpp"
#include <vector>
#include <filesystem>



class RLR : public CommonStats {
    public:
        // Constructors
        RLR();

        void Read_File(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row);

        void Encode_With_One_Nibble_Run_Length();
        void Decode_With_One_Nibble_Run_Length();

        void Encode_With_One_Byte_Run_Length();
        void Decode_With_One_Byte_Run_Length();

        void Encode_With_Two_Byte_Run_Length();
        void Decode_With_Two_Byte_Run_Length();

        void Encode_With_Three_Byte_Run_Length();
        void Decode_With_Three_Byte_Run_Length();

        void Encode_With_Four_Byte_Run_Length();
        void Decode_With_Four_Byte_Run_Length();

        void Encode_With_Five_Byte_Run_Length();
        void Decode_With_Five_Byte_Run_Length();

        void Encode_With_Burrow_Wheeler_Transformation_Little_Endian();
        void Decode_With_Inverse_Burrow_Wheeler_Transformation_Little_Endian();

        void Encode_With_Move_To_Front_Transformation();
        void Decode_With_Move_To_Front_Transformation();

        void Encode_With_Delta_Transformation();
        void Decode_With_Delta_Transformation();

        void Encode_With_XOR_Transformation();
        void Decode_With_XOR_Transformation();

        void Write_Compressed_File(const std::filesystem::path& file_path) const;
        void Write_Decompressed_File(const std::filesystem::path& file_path) const;


        //getters
        const char* Get_Compression_Type() const;
        const std::vector<char> Get_Encoded_Data_Vec() const;
        const std::vector<char> Get_Decoded_Data_Vec() const;
        const std::vector<char> Get_Binary_Data_Vec() const;



        //functions
        // void getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* decodedFilename, size_t fileSize, std::filesystem::path& currentDir);
        // Control_Stats getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir, CommonStats &localStats);


    private:
        const char* compression_type = "rlr_one_nibble";
        std::vector<char> binary_data_vec = {0};
        std::vector<char> encoded_data_vec = {0};
        std::vector<char> decoded_data_vec = {0};
        // std::vector<char> encoded_move_to_front_data_vec = {0};
        // std::vector<char> decoded_move_to_frontsquared_data_vec = {0};
        // std::vector<char> sentinel_vec = {0};
};