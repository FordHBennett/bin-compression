#pragma once

#include "../classes/common_stats.h"
#include "../classes/shannon_fano.h"
#include <filesystem>
#include <unordered_map>


class RLR;

std::ifstream Open_Input_File(const std::filesystem::path& file_path);

template <typename Predicate>
int Count_Files_With_Condition(const std::filesystem::path& dir_path, Predicate pred);

template <typename Predicate>
std::vector<std::filesystem::path> Get_Files_With_Condition(const std::filesystem::path& dir_path, Predicate pred);



const uint64_t Get_File_Size_Bytes(const std::filesystem::path& file_path);

const int Get_Number_Of_Geobin_Files_In_Directory(const std::filesystem::path& dir_path);

const int Get_Number_Of_Geometa_Files(const std::filesystem::path& dir_path);

const std::filesystem::path  Get_Geometa_File_Path(const std::filesystem::path& dir_path);

void Delete_Files_In_Directory(const std::filesystem::path& dir_path);

const std::vector<std::filesystem::path> Get_Geobin_File_Vec(const std::filesystem::path& dir_path);

const std::vector<std::filesystem::path> Get_Geobin_And_Geometa_Directory_Path_Vec(const std::filesystem::path& dir_path);

std::filesystem::path Remove_all_Seperators_From_Path(const std::filesystem::path& path);

const uint64_t Get_Side_Resolution(const std::filesystem::path& stem_path, CommonStats& stats_obj);

const int Get_Lod_Number(const std::filesystem::path& stem_path);

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files, RLR& rlr_obj);