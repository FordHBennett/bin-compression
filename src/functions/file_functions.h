#pragma once

#include <filesystem>
#include <iostream>

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


// class RLR;

std::ifstream Open_Input_File(const std::filesystem::path& file_path);

std::vector<char> Read_File(const std::filesystem::path& file_path, const int& number_of_bytes_to_read, const int& row);

template<typename T>
void Write_Compressed_File(const std::filesystem::path& file_path, T& binary_data){
    try {

        std::ofstream encoded_output_file(file_path, std::ios::binary | std::ios::app);

        encoded_output_file.write(binary_data.data(), binary_data.size());

        encoded_output_file.close();
    } catch (const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error: Unable to create the run-length encoded file.\n"} + std::string{"ERROR CODE: "} + std::string{e.what()});
    }
}

template <typename T>
void Write_Decompressed_File(const std::filesystem::path& file_path, T& binary_data){
    try{
        std::ofstream decoded_output_file(file_path, std::ios::binary | std::ios::app);

        decoded_output_file.write(binary_data.data(), binary_data.size());

        decoded_output_file.close();
    } catch (const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error: Unable to create the run-length decoded file.\n"} + std::string{"ERROR CODE: "} + std::string{e.what()});
    }
}

template <typename Predicate>
int Count_Files_With_Condition(const std::filesystem::path& dir_path, Predicate pred){
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        if (pred(entry)) {
            count++;
        }
    }
    return count;
}

template <typename Predicate>
std::vector<std::filesystem::path> Get_Files_With_Condition(const std::filesystem::path& dir_path, Predicate pred) {
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (pred(entry)) {
            files.push_back(entry.path());
        }
    }
    return files;
}



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