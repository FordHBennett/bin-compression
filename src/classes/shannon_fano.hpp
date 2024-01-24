#pragma once


#include "common_stats.hpp"
#include <vector>

class ShannonFano : public CommonStats {
    private:
        const char* compression_type = "ShannonFano";
        std::ifstream Open_File(const std::filesystem::path& path, std::ios::openmode mode) const;
        std::vector<char> Read_Binary_Data(std::ifstream& input_file, int number_of_bytes) const;
        template <typename T>
        std::pair<std::unordered_map<T, int>, std::vector<std::pair<T, int>>> Create_And_Sort_Frequency_Vector(const std::vector<char>& data) const;
        std::string Generate_Macro_Name(const std::filesystem::path& path) const;
    public:
        ShannonFano();
        void Write_Geobin_Data_As_Header_To_File(const std::filesystem::path& binary_path, const std::filesystem::path& header_path, const int& row_number, const int& number_of_bytes_to_read) const;
        void Write_Binary_Frequencies_Per_Row_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path,  const int& row_length, const uint64_t file_size) const;
        void Write_Binary_Frequencies_Per_File_To_Json_File(const std::filesystem::path& binary_path, const std::filesystem::path& json_path, const int& number_of_bytes_to_read) const;
        void Write_Lookup_Table_To_Header_File(const std::filesystem::path& binary_path, const std::filesystem::path& header_path, const int& number_of_bytes_to_read) const;
        std::string Format_Path_String(const std::filesystem::path& path) const;
        std::string Format_Byte_Pair(int byte_pair) const;
        std::string Format_Byte_Quartet(uint32_t byte_quartet) const;



};