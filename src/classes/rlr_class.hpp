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

        void Encode_With_One_Byte_Run_Length();
        void Decode_With_One_Byte_Run_Length();

        void Encode_With_Two_Byte_Run_Length();
        void Decode_With_Two_Byte_Run_Length();

        void Encode_With_One_Nibble_Run_Length();
        void Decode_With_One_Nibble_Run_Length();
        void Encode_With_One_Nibble_Run_Length_Squared();
        void Decode_With_One_Nibble_Run_Length_Squared();

        void Write_Compressed_File(const std::filesystem::path& file_path) const;
        void Write_Decompressed_File(const std::filesystem::path& file_path) const;
        void Write_Compressed_File_Squared(const std::filesystem::path& file_path) const;
        void Write_Decompressed_File_Squared(const std::filesystem::path& file_path) const;

        //getters
        const char* Get_Compression_Type() const;
        const std::vector<char> Get_Encoded_Data_Vec() const;
        const std::vector<char> Get_Decoded_Data_Vec() const;
        const std::vector<char> Get_Binary_Data_Vec() const;



        //functions
        // void getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* decodedFilename, size_t fileSize, std::filesystem::path& currentDir);
        // Control_Stats getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir, CommonStats &localStats);


    private:
        const char* compression_type = "RLR";
        std::vector<char> binary_data_vec;
        std::vector<char> encoded_data_vec;
        std::vector<char> decoded_data_vec;
        std::vector<char> encoded_squared_data_vec;
        std::vector<char> decoded_squared_data_vec;


};