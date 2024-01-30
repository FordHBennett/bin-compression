#include "file_functions.hpp"
#include "../classes/rlr_class.hpp"
#include "../classes/common_stats.hpp"
#include "../classes/shannon_fano.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
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

#define MAX_CHUNK_SIZE (1<<15)



using json = nlohmann::json;

std::ifstream Open_Input_File(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        ERROR_MSG_AND_EXIT("Error: Unable to open file " + file_path.string());
    }
    return file;
}

template <typename Predicate>
int Count_Files_With_Condition(const std::filesystem::path& dir_path, Predicate pred) {
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


void Generate_Random_Binary_File(const char* filename, long long fileSize, double zeroProbability) {
    // std::ofstream outFile(filename, std::ios::binary);
    // if (!outFile) {
    //     std::cout << "Error: Unable to open the file for writing." << '\n';
    //     return;
    // }

    // // Determine the number of zero bytes to write
    // long long zeroBytes = static_cast<long long>(zeroProbability * fileSize);

    // for (long long i = 0; i < zeroBytes; ++i) {
    //     char byte = 0;  // Write zero bytes
    //     outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    // }

    // // Determine the number of non-zero bytes to write
    // long long nonZeroBytes = fileSize - zeroBytes;

    // std::default_random_engine generator;
    // std::bernoulli_distribution distribution(0.5); // 50% probability for non-zero bits

    // for (long long i = 0; i < nonZeroBytes; ++i) {
    //     bool bit = distribution(generator);
    //     char byte = static_cast<char>(bit);
    //     outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    // }

    // outFile.close();
}


const uint64_t Get_File_Size_Bytes(const std::filesystem::path& file_path)
{
    try {
        return std::filesystem::file_size(file_path);
    } catch(const std::filesystem::filesystem_error& e) {
        ERROR_MSG_AND_EXIT(std::string{"ERROR: Error accessing file size for"} + file_path.string() + std::string{" : "} + std::string{e.what()});
    }

}

// Function to return the number of Geobin files in a directory
const int Get_Number_Of_Geobin_Files_In_Directory(const std::filesystem::path& dir_path) {
    return Count_Files_With_Condition(dir_path, [](const auto& entry) {
        return entry.is_regular_file() && entry.path().extension() == ".geobin";
    });
}

const int Get_Number_Of_Geometa_Files(const std::filesystem::path& dir_path) {
    return Count_Files_With_Condition(dir_path, [](const auto& entry) {
        return entry.is_regular_file() && entry.path().extension() == ".geometa";
    });
}


const std::filesystem::path Get_Geometa_File_Path(const std::filesystem::path& dir_path) {
    auto files = Get_Files_With_Condition(dir_path, [](const auto& entry) {
        return entry.is_regular_file() && entry.path().extension() == ".geometa";
    });
    if (!files.empty()) {
        return files.front();
    }
    ERROR_MSG_AND_EXIT("Error: No .geometa file found in " + dir_path.string());
}


void Delete_Files_In_Directory(const std::filesystem::path& dir_path) {
    try {
        for(const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if(std::filesystem::is_regular_file(entry.status())) {
                std::filesystem::remove(entry.path());
            }
        }
    } catch(const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error deleting files in directory " + dir_path.string() + ": " + std::string{e.what()}});
    }
}

const std::vector<std::filesystem::path> Get_Geobin_File_Vec(const std::filesystem::path& dir_path) {
    return Get_Files_With_Condition(dir_path, [](const auto& entry) {
        return entry.is_regular_file() && entry.path().extension() == ".geobin";
    });
}

const std::vector<std::filesystem::path> Get_Geobin_And_Geometa_Directory_Path_Vec(const std::filesystem::path& dir_path) {
    return Get_Files_With_Condition(dir_path, [](const auto& entry) {
        return entry.is_directory() && Get_Number_Of_Geobin_Files_In_Directory(entry.path()) > 0 && Get_Number_Of_Geometa_Files(entry.path()) > 0;
    });
}


std::filesystem::path Remove_all_Seperators_From_Path(const std::filesystem::path& path) {
    std::string path_string = path.string();
    std::replace(path_string.begin(), path_string.end(), '/', '-');
    std::replace(path_string.begin(), path_string.end(), '\\', '-');
    return std::filesystem::path{path_string};
}

const uint64_t Get_Side_Resolution(const std::filesystem::path& stem_path, CommonStats& stats_obj) {
    auto extract_character_after = [](const std::filesystem::path& path, const std::string& delimiter) -> const std::string {
    const std::string filename = path.filename().string();
    size_t pos = filename.rfind(delimiter);
    if (pos != std::string::npos) {
        size_t start = pos + delimiter.length();
        while ((start < filename.length()) && !std::isdigit(filename[start])) {
            start++;
        }
        size_t end = start;
        while ((end < filename.length()) && std::isdigit(filename[end])) {
            end++;
        }

        if (start < end) {
            return filename.substr(start, end - start);
        }
    }
#ifdef DEBUG_MODE
    ERROR_MSG_AND_EXIT(std::string{"ERROR: Unable to extract number after delimiter: " + delimiter});
#endif
};


    const uint8_t lod_number = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_lod"})));
#ifdef DEBUG_MODE
    const uint8_t side = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_s"})));
    const uint8_t c_number = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_c"})));
    PRINT_DEBUG(std::string{"Side: "} + std::to_string(side));
    PRINT_DEBUG(std::string{"C Number: "} + std::to_string(c_number));
    PRINT_DEBUG(std::string{"LOD Number: "} + std::to_string(lod_number));
#endif

    // const uint side_resolution = stats_obj.Get_Side_Resolution(side, c_number);
    const uint64_t side_resolution = stats_obj.Get_Side_Resolution(lod_number);

#ifdef DEBUG_MODE
    PRINT_DEBUG(std::string{"Side Resolution: " + std::to_string(side_resolution)});
    PRINT_DEBUG(std::string{"Data Type Size : " + std::to_string(stats_obj.Get_Data_Type_Size())});
#endif

    return side_resolution;
}

const int Get_Lod_Number(const std::filesystem::path& stem_path) {
    auto extract_character_after = [](const std::filesystem::path& path, const std::string& delimiter) -> const std::string {
    const std::string filename = path.filename().string();
    size_t pos = filename.rfind(delimiter);
    if (pos != std::string::npos) {
        size_t start = pos + delimiter.length();
        while ((start < filename.length()) && !std::isdigit(filename[start])) {
            start++;
        }
        size_t end = start;
        while ((end < filename.length()) && std::isdigit(filename[end])) {
            end++;
        }

        if (start < end) {
            return filename.substr(start, end - start);
        }
    }
#ifdef DEBUG_MODE
    ERROR_MSG_AND_EXIT(std::string{"ERROR: Unable to extract number after delimiter: " + delimiter});
#endif
};

    return std::stoi(extract_character_after(stem_path, std::string{"_lod"}));
}

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files_vec, RLR& rlr_obj) {
#ifdef DEBUG_MODE
    assert(std::filesystem::equivalent(files_vec[0].parent_path(), files_vec.at(0).parent_path()));
#endif

    rlr_obj.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files_vec.at(0).parent_path()));

    for(const auto& file : files_vec) {
        const uint64_t file_size = Get_File_Size_Bytes(file);


#ifdef DEBUG_MODE
        PRINT_DEBUG(std::string{"File to be compressed: " + file.string()});
#endif
        const std::filesystem::path stem_path = file.stem();
        const std::filesystem::path encoded_file_path = file.parent_path() / std::filesystem::path{"compressed_decompressed_rlr_files"} /
                                                        stem_path / std::filesystem::path{(stem_path.string() + std::string{'.'} + std::string{rlr_obj.Get_Compression_Type()} + std::string{"_encoded"})};
        const std::filesystem::path decoded_file_path = file.parent_path() / std::filesystem::path{"compressed_decompressed_rlr_files"} /
                                                        stem_path / std::filesystem::path{(stem_path.string() + std::string{'.'} + std::string{rlr_obj.Get_Compression_Type()} + std::string{"_decoded"})};

        if(!std::filesystem::exists(encoded_file_path.parent_path())) {
            std::filesystem::create_directories(encoded_file_path.parent_path());
        }

        Delete_Files_In_Directory(encoded_file_path.parent_path());

        const uint64_t side_resolution = Get_Side_Resolution(stem_path, rlr_obj);
        uint64_t bytes_per_row = side_resolution * rlr_obj.Get_Data_Type_Size();
        rlr_obj.number_of_bytes_per_row = bytes_per_row;
        uint64_t num_rows = file_size / bytes_per_row;


#ifdef DEBUG_MODE
        PRINT_DEBUG(std::string{"Number of rows: " + std::to_string(num_rows)});
        PRINT_DEBUG(std::string{"Bytes per row: " + std::to_string(bytes_per_row)});
        if(Get_File_Size_Bytes(file) % bytes_per_row != 0) {
            PRINT_DEBUG(std::string{"ERROR: File size is not a multiple of the number of bytes per row."});
            PRINT_DEBUG(std::string{"ERROR: File size: " + std::to_string(Get_File_Size_Bytes(file))});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row: " + std::to_string(bytes_per_row)});
            PRINT_DEBUG(std::string{"ERROR: This probably means that rlr_obj.data_type_size is wrong for the file that is being commpressed"});
            PRINT_DEBUG(std::string{"ERROR: You are trying to compress " + file.string()});
            // PRINT_DEBUG(std::string{"ERROR: Side Number is: " + extract_character_after(stem_path, "_s")});
            // PRINT_DEBUG(std::string{"ERROR: C Number is: " + extract_character_after(stem_path, "_c")});
            PRINT_DEBUG(std::string{"ERROR: Side Resolution is: " + std::to_string(side_resolution)});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row is: " + std::to_string(bytes_per_row)});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row is: " + std::to_string(bytes_per_row)});
            ERROR_MSG_AND_EXIT(std::string{"ERROR:"});
        }
#endif
        for(int iteration = 0; iteration < rlr_obj.Get_Number_Of_Iterations(); iteration++){
            for(uint64_t row = 0; row<num_rows; row++){
                rlr_obj.row_number = row;
                rlr_obj.Read_File(file, bytes_per_row, row);

                // rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                //     rlr_obj.Encode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length();
                // });


                rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                    rlr_obj.Encode_With_One_Byte_Run_Length();
                });

                // rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                //     rlr_obj.Encode_With_XOR_Transformation();
                // });

                rlr_obj.Write_Compressed_File(encoded_file_path);

                rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                    rlr_obj.Decode_With_One_Byte_Run_Length();
                });



                // rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                //     rlr_obj.Decode_With_XOR_Transformation();
                // });



                // rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                //     rlr_obj.Decode_With_Move_To_Front_Transformation_With_One_Byte_Run_Length();
                // });

                rlr_obj.Write_Decompressed_File(decoded_file_path);

                if(!rlr_obj.Is_Decoded_Data_Equal_To_Original_Data(rlr_obj.Get_Decoded_Data_Vec(), rlr_obj.Get_Binary_Data_Vec())){
                    ERROR_MSG_AND_EXIT(std::string{"ERROR: Decoded data is not equal to original data."});
                }
            }
            rlr_obj.Compute_Compression_Ratio(file, encoded_file_path);
            rlr_obj.Compute_Compressed_File_Size(encoded_file_path);
            // if(!(iteration == number_of_iterations - 1)) {
            Delete_Files_In_Directory(encoded_file_path.parent_path());
            // }
        }
        std::filesystem::remove_all(encoded_file_path.parent_path().parent_path());
    }
}

void Write_Shannon_Fano_Frequencies_To_Files(const std::vector<std::filesystem::path>& files, ShannonFano& shannon_fano) {
    shannon_fano.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files.at(0).parent_path()));

    for(const auto& file : files) {
        const std::filesystem::path stem_path = file.stem();
        const std::filesystem::path json_path = std::filesystem::path{"shannon_fano_frequency_files"} /
                                                        file.parent_path() / std::filesystem::path{stem_path.string() + std::string{".json"}};

        if(!std::filesystem::exists(json_path.parent_path())) {
            std::filesystem::create_directories(json_path.parent_path());
        }
        // const int lod_number = Get_Lod_Number(stem_path);
        const uint64_t side_resolution = Get_Side_Resolution(stem_path, shannon_fano);
        uint64_t bytes_per_row = side_resolution * shannon_fano.Get_Data_Type_Size();
        uint64_t num_rows = Get_File_Size_Bytes(file) / bytes_per_row;


        // if(shannon_fano.Get_Data_Type_Size() == 4) {
        //     shannon_fano.Write_Binary_Frequencies_Per_File_To_Json_File(file, json_path,  Get_File_Size_Bytes(file));
        // } else {
        //     // for(uint64_t row = 0; row<num_rows; row++){
        //         shannon_fano.Write_Binary_Frequencies_Per_File_To_Json_File(file, json_path, bytes_per_row, Get_File_Size_Bytes(file));
        //     // }
        // }
        // shannon_fano.Write_Binary_Frequencies_Per_File_To_Json_File(file, json_path, bytes_per_row, Get_File_Size_Bytes(file));
        shannon_fano.Write_Binary_Frequencies_Per_File_To_Json_File(file, json_path, Get_File_Size_Bytes(file));


    }
}

void Write_Shannon_Fano_Lookup_Table_To_Files(const std::vector<std::filesystem::path>& files, ShannonFano& shannon_fano) {
    shannon_fano.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files.at(0).parent_path()));

    for(const auto& file : files) {
        const std::filesystem::path stem_path = file.stem();
        const std::filesystem::path header_path = std::filesystem::path{"PlanetData_LUT"} /
                                                        std::filesystem::path{file.parent_path().string() + std::string{".h"}};



        if(!std::filesystem::exists(header_path.parent_path())) {
            std::filesystem::create_directories(header_path.parent_path());
        }
        // const int lod_number = Get_Lod_Number(stem_path);
        const uint64_t side_resolution = Get_Side_Resolution(stem_path, shannon_fano);
        uint64_t bytes_per_row = side_resolution * shannon_fano.Get_Data_Type_Size();
        uint64_t num_rows = Get_File_Size_Bytes(file) / bytes_per_row;



        // if(shannon_fano.Get_Data_Type_Size() == 4) {
        //     shannon_fano.Write_Lookup_Table_To_Header_File(file, json_path,  Get_File_Size_Bytes(file));
        // } else {
        //     // for(uint64_t row = 0; row<num_rows; row++){
        //         shannon_fano.Write_Lookup_Table_To_Header_File(file, json_path, bytes_per_row, Get_File_Size_Bytes(file));
        //     // }
        // }

        shannon_fano.Write_Lookup_Table_To_Header_File(file, header_path,  Get_File_Size_Bytes(file));
    }
}