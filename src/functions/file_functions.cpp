#include "file_functions.hpp"
#include "../classes/rlr_class.hpp"
#include "../classes/common_stats.hpp"
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



using json = nlohmann::json;

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
        ERROR_MSG_AND_EXIT(std::string{"ERROR: Error accessing file size for"} + file_path.string() + ": " + std::string{e.what()});
    }

}

// Function to return the number of Geobin files in a directory
const int Get_Number_Of_Geobin_Files_Recursively(const std::filesystem::path& dir_path) {
    try {
        int count = 0;

        for(const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".geobin") {
                ++count;
            }
        }

        return count;

    } catch(const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"ERROR: " + std::string{e.what()}});
    }
}

const int Get_Number_Of_Geometa_Files(const std::filesystem::path& dir_path){
    try {
        int count = 0;

        for(const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geometa") {
                count++;
            }
        }

        return count;
    }
    catch(const std::exception& e) {
        // std::cerr << "ERROR: " << e.what() << '\n';
        // PRINT_DEBUG(std::string{"ERROR: " + std::string{e.what()}});
        ERROR_MSG_AND_EXIT(std::string{"ERROR: "} + std::string{e.what()});
    }
}

const std::filesystem::path  Get_Geometa_File_Path(const std::filesystem::path& dir_path) {

    try{
        for(const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if(std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geometa") {
                return entry.path();
            }
        }
    } catch(const std::exception& e) {
        // PRINT_DEBUG(std::string{"Error accessing geometa file for " + dir_path.string() + ": " + std::string{e.what()}});
        ERROR_MSG_AND_EXIT(std::string{"Error accessing geometa file for " + dir_path.string() + ": " + std::string{e.what()}});
    }

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
    try {
        std::vector<std::filesystem::path> geobin_files_vec;

        for(const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if(std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
                geobin_files_vec.push_back(entry.path());
            }
        }

        return geobin_files_vec;
    } catch(const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error accessing geobin files in directory " + dir_path.string() + ": " + std::string{e.what()}});
    }
}

const std::vector<std::filesystem::path> Get_Geobin_And_Geometa_Directory_Path_Vec(const std::filesystem::path& dir_path) {
    try {
        std::vector<std::filesystem::path> geobin_and_geometa_directory_path_vec;

        for(const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if(std::filesystem::is_directory(entry.status())) {
                if(Get_Number_Of_Geobin_Files_Recursively(entry.path()) > 0 && Get_Number_Of_Geometa_Files(entry.path()) > 0) {
                    geobin_and_geometa_directory_path_vec.push_back(entry.path());
#ifdef DEBUG_MODE
                    PRINT_DEBUG(std::string{"Directory with geobin and geometa files: " + entry.path().string()});
#endif
                }
            }
        }

        return geobin_and_geometa_directory_path_vec;
    } catch(const std::exception& e) {
        ERROR_MSG_AND_EXIT(std::string{"Error accessing geobin files in directory " + dir_path.string() + ": " + std::string{e.what()}});
    }
}

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files_vec, const int& number_of_iterations, RLR& rlr_obj) {
#ifdef DEBUG_MODE
    assert(std::filesystem::equivalent(files_vec[0].parent_path(), files_vec.at(0).parent_path()));
#endif

    rlr_obj.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files_vec.at(0).parent_path()));

    for(const auto& file : files_vec) {

#ifdef DEBUG_MODE
        PRINT_DEBUG(std::string{"File to be compressed: " + file.string()});
#endif
        const std::filesystem::path stem_path = file.stem();
        const std::filesystem::path encoded_file_path = file.parent_path() / "compressed_decompressed_rlr_files" / stem_path / (stem_path.string() + ".rlr_encoded_bin");
        const std::filesystem::path decoded_file_path = file.parent_path() /  "compressed_decompressed_rlr_files" / stem_path / (stem_path.string() + ".rlr_decoded_bin");
        if(!std::filesystem::exists(encoded_file_path.parent_path())) {
            std::filesystem::create_directories(encoded_file_path.parent_path());
        }

        if(!std::filesystem::exists(decoded_file_path.parent_path())) {
            std::filesystem::create_directories(decoded_file_path.parent_path());
        }
        Delete_Files_In_Directory(encoded_file_path.parent_path());

        // const std::filesystem::path encoded_squared_file_path = file.parent_path() / "compressed_decompressed_rlr_files" / stem_path / "compressed_decompressed_squared_rlr_files"/ (stem_path.string() + ".rlr_encoded_bin");
        // const std::filesystem::path decoded_squared_file_path = file.parent_path() /  "compressed_decompressed_rlr_files" / stem_path / "compressed_decompressed_squared_rlr_files"/ (stem_path.string() + ".rlr_decoded_bin");
        // if(!std::filesystem::exists(encoded_squared_file_path.parent_path())) {
        //     std::filesystem::create_directories(encoded_squared_file_path.parent_path());
        // }

        // if(!std::filesystem::exists(decoded_squared_file_path.parent_path())) {
        //     std::filesystem::create_directories(decoded_squared_file_path.parent_path());
        // }
        // Delete_Files_In_Directory(encoded_squared_file_path.parent_path());


        //lod could be over 9
        //scan until you find non-numeric
        // auto extract_character_after = [](const std::filesystem::path& path, const char* delimiter) -> const char {
        //     const std::string filename = path.filename().string();
        //     size_t pos = path.filename().string().rfind(delimiter);
        //     if (pos != std::string::npos && pos + 1 < filename.length()) {
        //         return filename[pos + std::string{delimiter}.length()];
        //     }
        //     // std::cerr << "ERROR: Unable to extract character after delimiter: " << delimiter << '\n';
        //     ERROR_MSG_AND_EXIT(std::string{ "ERROR: Unable to extract character after delimiter: " + std::string{delimiter}});
        // };
        auto extract_character_after = [](const std::filesystem::path& path, const std::string& delimiter) -> const std::string {
        const std::string filename = path.filename().string();
        size_t pos = filename.rfind(delimiter);
        if (pos != std::string::npos) {
            size_t start = pos + delimiter.length();
            // Find the start of the number by skipping any non-digit characters
            while (start < filename.length() && !std::isdigit(filename[start])) {
                ++start;
            }
            // Find the end of the number
            size_t end = start;
            while (end < filename.length() && std::isdigit(filename[end])) {
                ++end;
            }
            // Extract the number substring
            if (start < end) {
                return filename.substr(start, end - start);
            }
        }
    // Error handling if the number is not found
    ERROR_MSG_AND_EXIT(std::string{"ERROR: Unable to extract number after delimiter: " + delimiter});
};


        const uint8_t lod_number = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_lod"})));
#ifdef DEBUG_MODE
        const uint8_t side = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_s"})));
        const uint8_t c_number = static_cast<uint8_t>(std::stoi(extract_character_after(stem_path, std::string{"_c"})));
        PRINT_DEBUG(std::string{"Side: "} + std::to_string(side));
        PRINT_DEBUG(std::string{"C Number: "} + std::to_string(c_number));
        PRINT_DEBUG(std::string{"LOD Number: "} + std::to_string(lod_number));
#endif

        // const uint side_resolution = rlr_obj.Get_Side_Resolution(side, c_number);
        const int64_t side_resolution = rlr_obj.Get_Side_Resolution(lod_number);

#ifdef DEBUG_MODE
        PRINT_DEBUG(std::string{"Side Resolution: " + std::to_string(side_resolution)});
#endif

        const int64_t bytes_per_row = side_resolution * rlr_obj.Get_Data_Type_Size();
        const int num_rows = static_cast<int64_t>(Get_File_Size_Bytes(file) / bytes_per_row);

#ifdef DEBUG_MODE
        if(Get_File_Size_Bytes(file) % bytes_per_row != 0) {
            PRINT_DEBUG(std::string{"ERROR: File size is not a multiple of the number of bytes per row."});
            PRINT_DEBUG(std::string{"ERROR: File size: " + std::to_string(Get_File_Size_Bytes(file))});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row: " + std::to_string(bytes_per_row)});
            PRINT_DEBUG(std::string{"ERROR: This probably means that rlr_obj.data_type_size is wrong for the file that is being commpressed"});
            PRINT_DEBUG(std::string{"ERROR: You are trying to compress " + file.string()});
            PRINT_DEBUG(std::string{"ERROR: Side Number is: " + std::string{extract_character_after(stem_path, "_s")}});
            PRINT_DEBUG(std::string{"ERROR: C Number is: " + std::string{extract_character_after(stem_path, "_c")}});
            PRINT_DEBUG(std::string{"ERROR: Side Resolution is: " + std::to_string(side_resolution)});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row is: " + std::to_string(bytes_per_row)});
            PRINT_DEBUG(std::string{"ERROR: Bytes per row is: " + std::to_string(bytes_per_row)});
            ERROR_MSG(std::string{"ERROR:"});
        }
#endif

        for(int iteration = 0; iteration < number_of_iterations; iteration++){
            for(int row = 0; row<num_rows; row++){
                rlr_obj.Read_File(file, bytes_per_row, row);
                rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                    rlr_obj.Encode_With_One_Nibble_Run_Length();
                });
                rlr_obj.Write_Compressed_File(encoded_file_path);

                // rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                //     rlr_obj.Encode_With_One_Nibble_Run_Length_Squared();
                // });
                // rlr_obj.Write_Compressed_File_Squared(encoded_squared_file_path);

                // rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                //     rlr_obj.Decode_With_One_Nibble_Run_Length_Squared();
                // });
                // rlr_obj.Write_Decompressed_File_Squared(decoded_squared_file_path);

                rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                    rlr_obj.Decode_With_One_Nibble_Run_Length();
                });
                rlr_obj.Write_Decompressed_File(decoded_file_path);

                if(!rlr_obj.Is_Decoded_Data_Equal_To_Original_Data(rlr_obj.Get_Decoded_Data_Vec(), rlr_obj.Get_Binary_Data_Vec())){
                    ERROR_MSG_AND_EXIT(std::string{"ERROR: Decoded data is not equal to original data."});
                }
            }
            rlr_obj.Compute_Compression_Ratio(file, encoded_file_path);
            rlr_obj.Compute_Compressed_File_Size(encoded_file_path);
            // if(!(iteration == number_of_iterations - 1)) {
            //     Delete_Files_In_Directory(encoded_file_path.parent_path());
            //     // Delete_Files_In_Directory(encoded_squared_file_path.parent_path());
            // }
        }
        std::filesystem::remove_all(encoded_file_path.parent_path().parent_path());
    }
}
