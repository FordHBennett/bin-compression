#pragma once


#include "common_stats.hpp"
#include <vector>

class ShannonFano : public CommonStats {
    public:
        ShannonFano();
        void Write_Geobin_Data_As_Header_To_File(const std::filesystem::path& binary_path, const std::filesystem::path& header_path, const int& row_number, const int& number_of_bytes_to_read) const;
        void Write_Binary_Frequencies_Per_Row_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& row_number, const int& number_of_bytes_to_read) const;
        void Write_Binary_Frequencies_Per_File_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& number_of_bytes_to_read) const;


    private:
        const char* compression_type = "ShannonFano";
};