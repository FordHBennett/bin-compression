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
        // std::cerr << "Error accessing file size for " << file_path << ": " << e.what() << '\n';
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
        // std::cerr << "ERROR: " << e.what() << '\n';
        // PRINT_DEBUG(std::string{"ERROR: " + std::string{e.what()}});
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
                // std::cerr << "You have found a the geometa file: " << entry.path().c_str() << '\n';
                // PRINT_DEBUG(std::string{"You have found a the geometa file: " + entry.path().string()});
                return entry.path();
            }
        }
    } catch(const std::exception& e) {
        // PRINT_DEBUG(std::string{"Error accessing geometa file for " + dir_path.string() + ": " + std::string{e.what()}});
        ERROR_MSG_AND_EXIT(std::string{"Error accessing geometa file for " + dir_path.string() + ": " + std::string{e.what()}});
    }

}

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Control_Stats& avgTotalControlStats) {

//     for (const auto& file : files) {
//         std::cout << "RLR: " <<file << '\n';
//         std::filesystem::path currentDir = file.parent_path();
//         //std::cout << currentDir << '\n';
//         // avgTotalRLRStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);

//         RLR rlr_obj;
//         rlr_obj.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir, avgTotalControlStats);
//     }
// }

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZW_Stats& avgTotalLZWStats) {

//     for (const auto& file : files) {
//         std::cout << "LZW: " << file << '\n';
//         std::filesystem::path currentDir = file.parent_path();
//         std::string encodedFilename =  std::string(file.path()).erase(std::string(file).size()-7,6) + ".run_length_encoded_bin";
//         std::string dencodedFilename = std::string(file).erase(std::string(file).size()-7,6)  + ".run_length_decoded_bin";
//         for (int i = 0; i < numIterations; ++i) {
//             std::ifstream inFile(filename, std::ios::binary);
//             if (!inFile) {
//                 std::cout << "Error" << '\n';
//             }

//             inFile.seekg(0, std::ios::end);
//             size_t fileSize = inFile.tellg();
//             inFile.seekg(0, std::ios::beg);

//             std::vector<char> binaryData(fileSize);
//             inFile.read(binaryData.data(), fileSize);
//             inFile.close();

//             avgTotalLZWStats.computeFileStats(binaryData, encodedFilename.c_str(), dencodedFilename.c_str(), fileSize, currentDir);
//         }

//     }
// }

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZP_Stats& avgTotalLZPStats) {

//     for (const auto& file : files) {
//         std::cout << "LZP: " << file << '\n';
//         std::filesystem::path currentDir = file.parent_path();
//         avgTotalLZPStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);
//     }
// }

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Huffman_Stats& avgTotalHuffmanStats) {

//     for (const auto& file : files) {
//         std::cout << file << '\n';
//         std::filesystem::path currentDir = file.parent_path();
//         avgTotalHuffmanStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);
//     }
// }


// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Control_Stats& avgTotalControlStats) {

//     for (const auto& file : files) {
//         // std::cout << "Control: " << file << '\n';
//         // std::filesystem::path currentDir = file.parent_path();
//         // avgTotalControlStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);
//     }
// }

// void computeFileStats(std::vector<char> &binaryData, const char* encodedFilename,
//                         const char* decodedFilename, size_t fileSize,
//                         std::filesystem::path& currentDir, RLR& rlr_obj, CommonStats &localStats) {
//     // Perform run-length encoding
//     auto startEncod = std::chrono::high_resolution_clock::now();
//     auto encoded = rlr_obj.encode(binaryData, datatype_size);
//     auto stopEncode = std::chrono::high_resolution_clock::now();
//     auto durationEncode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopEncode - startEncod);

//     // Perform run-length decoding
//     auto startDecode = std::chrono::high_resolution_clock::now();
//     auto decoded = rlr_obj.decode(encoded, datatype_size);
//     auto stopDecode = std::chrono::high_resolution_clock::now();
//     auto durationDecode = std::chrono::duration_cast<std::chrono::nanoseconds>(stopDecode - startDecode);

//     // Calculate the peak memory usage
//     // avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
//     // avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

//     // Verify that no data is lost by comparing decoded data with the original data
//     bool dataMatches = binaryData == decoded;
//     assert(dataMatches);
//     //std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << '\n';

//     // Write the encoded and decoded data to files
//     rlr_obj.Write_Compressed_File(encoded, encodedFilename);
//     rlr_obj.Write_Decompressed_File(decoded, decodedFilename);
// }

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files_vec, const int& numIterations, RLR& rlr_obj) {
#ifdef DEBUG_MODE
    assert(std::filesystem::equivalent(files_vec[0].parent_path(), files_vec.at(0).parent_path()));
#endif

    rlr_obj.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files_vec.at(0).parent_path()));

    for(const auto& file : files_vec) {
// #ifdef DEBUG_MODE
//         if(file == files_vec.at(3)) {
//             break;;
//         }
// #endif

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

        //lod could be over 9
        //scan until you find non-numeric
        auto extract_character_after = [](const std::filesystem::path& path, const char* delimiter) -> const char {
            const std::string filename = path.filename().string();
            size_t pos = path.filename().string().rfind(delimiter);
            if (pos != std::string::npos && pos + 1 < filename.length()) {
                return filename[pos + std::string{delimiter}.length()];
            }
            // std::cerr << "ERROR: Unable to extract character after delimiter: " << delimiter << '\n';
            ERROR_MSG_AND_EXIT(std::string{ "ERROR: Unable to extract character after delimiter: " + std::string{delimiter}});
        };

        const uint8_t lod_number = static_cast<uint8_t>(extract_character_after(stem_path, "_lod") - '0');
#ifdef DEBUG_MODE
        const uint8_t side = static_cast<uint8_t>(extract_character_after(stem_path, "_s") - '0');
        const uint8_t c_number = static_cast<uint8_t>(extract_character_after(stem_path, "_c") - '0');
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

        for(int iteration = 0; iteration < numIterations; iteration++){
            for(int row = 0; row<num_rows; row++){
                rlr_obj.Read_File(file, bytes_per_row, row);
                // rlr_obj.Encode_With_One_Byte_Run_Length();
                // rlr_obj.Decode_With_One_Byte_Run_Length();
                // rlr_obj.Encode_With_One_Nibble_Run_Length();
                // rlr_obj.Decode_With_One_Nibble_Run_Length();
                rlr_obj.Compute_Time_Encoded([&rlr_obj](){
                    rlr_obj.Encode_With_One_Nibble_Run_Length();
                });

                rlr_obj.Compute_Time_Decoded([&rlr_obj](){
                    rlr_obj.Decode_With_One_Nibble_Run_Length();
                });
                rlr_obj.Write_Compressed_File(encoded_file_path);
                rlr_obj.Write_Decompressed_File(decoded_file_path);
#ifdef DEBUG_MODE
                if(!rlr_obj.Is_Decoded_Data_Equal_To_Original_Data(rlr_obj.Get_Decoded_Data_Vec(), rlr_obj.Get_Binary_Data_Vec())){
                    // std::cerr << "ERROR: Decoded data is not equal to original data." << '\n';
                    ERROR_MSG_AND_EXIT(std::string{"ERROR: Decoded data is not equal to original data."});
                }
#endif
            }
            rlr_obj.Compute_Compression_Ratio(file, encoded_file_path);
            rlr_obj.Compute_File_Size(encoded_file_path);

        }
    }
}
