#pragma once

#include "../classes/common_stats.hpp"
#include "../classes/shannon_fano.hpp"
#include <filesystem>
#include <unordered_map>


class RLR;
// class LZW_Stats;
// class LZP_Stats;
// class Huffman_Stats;
// class LZO1_Stats;
// class Control_Stats;


void Generate_Random_Binary_File(const char* filename, long long fileSize, double zeroProbability);

const uint64_t Get_File_Size_Bytes(const std::filesystem::path& file_path);

const int Get_Number_Of_Geobin_Files_In_Directory(const std::filesystem::path& dir_path);

const int Get_Number_Of_Geometa_Files(const std::filesystem::path& dir_path);

const std::filesystem::path  Get_Geometa_File_Path(const std::filesystem::path& dir_path);

void Delete_Files_In_Directory(const std::filesystem::path& dir_path);

const std::vector<std::filesystem::path> Get_Geobin_File_Vec(const std::filesystem::path& dir_path);

const std::vector<std::filesystem::path> Get_Geobin_And_Geometa_Directory_Path_Vec(const std::filesystem::path& dir_path);

std::filesystem::path Remove_all_Seperators_From_Path(const std::filesystem::path& path);

const uint64_t Get_Side_Resolution(const std::filesystem::path& stem_path, RLR& rlr_obj);

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files, RLR& rlr_obj);

void Write_Shannon_Fano_Frequencies_To_Files(const std::vector<std::filesystem::path>& files, ShannonFano& shannon_fano);