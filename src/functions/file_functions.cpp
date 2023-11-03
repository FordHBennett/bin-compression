#include "file_functions.h"
#include "../classes/rlr_class.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#define ERROR_MSG(msg) \
    std::cerr << msg << " OCCURED IN: " << "\n"; \
    std::cerr << "      File: " << __FILE__ << "\n"; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << "\n"; \
    std::cerr << "      Line: " << __LINE__ << "\n"; \


using json = nlohmann::json;

void Generate_Random_Binary_File(const char* filename, long long fileSize, double zeroProbability) {
    // std::ofstream outFile(filename, std::ios::binary);
    // if (!outFile) {
    //     std::cout << "Error: Unable to open the file for writing." << "\n";
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


const uint64_t Get_File_Size_Bytes(const std::filesystem::path file_path)
{
    // try {
    //     return std::filesystem::file_size(file_path);
    // } catch (const std::filesystem::filesystem_error& e) {
    //     std::cerr << "Error accessing file size for " << file_path << ": " << e.what() << '\n';
    //     ERROR_MSG("ERROR");
    // }

    // calculate the file size
    std::ifstream in_file(file_path, std::ios::binary);
    if(!in_file) {
        std::cerr << "ERROR: Unable to open file: " << file_path.c_str() << "\n";
        ERROR_MSG("ERROR");
        return -1;
    }

    in_file.seekg(0, std::ios::end);
    const uint64_t file_size = in_file.tellg();
    in_file.close();

    return file_size;
}

// Function to return the number of Geobin files in a directory
const int Get_Number_Of_Geobin_Files_Recursively(const std::filesystem::path dir_path) {
    try {
        if (!std::filesystem::exists(dir_path)) {
            std::cerr << "ERROR: Directory does not exist: " << dir_path.c_str() << "\n";
            ERROR_MSG("ERROR");
            return -1;
        }

        int count = 0;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
                count++;
            }
        }

        return count;
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        ERROR_MSG("ERROR");
        return -1;
    }
}

const int Get_Number_Of_Geometa_Files(const std::filesystem::path dir_path){
    try {

        if (!std::filesystem::exists(dir_path)) {
            std::cerr << "ERROR: Directory does not exist: " << dir_path.c_str() << "\n";
            ERROR_MSG("ERROR");
            return -1;
        }

        int count = 0;

        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geometa") {
                count++;
            }
        }

        return count;
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        ERROR_MSG("ERROR");
        return -1;
    }

    return -1;
}

const std::filesystem::path  Get_Geometa_File_Path(const std::filesystem::path dir_path) {

    if(!std::filesystem::exists(dir_path)) {
        std::cerr << "ERROR: Directory does not exist: " << dir_path.c_str() << "\n";
        ERROR_MSG("ERROR");
        throw std::runtime_error("Directory does not exist");
    }
    else if(!std::filesystem::is_directory(dir_path)) {
        std::cerr << "ERROR: " << dir_path.c_str() << " is not a directory." << "\n";
        ERROR_MSG("ERROR");
        throw std::runtime_error("Not a directory");
    }
    else if(std::filesystem::is_empty(dir_path)) {
        std::cerr << "ERROR: " << dir_path.c_str() << " is empty." << "\n";
        ERROR_MSG("ERROR");
        throw std::runtime_error("Directory is empty");
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geometa") {
            std::cerr << "You have found a the geometa file: " << entry.path().c_str() << "\n";
            return entry.path();
        }
    }

}

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Control_Stats& avgTotalControlStats) {

//     for (const auto& file : files) {
//         std::cout << "RLR: " <<file << "\n";
//         std::filesystem::path currentDir = file.parent_path();
//         //std::cout << currentDir << "\n";
//         // avgTotalRLRStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);

//         RLR rlr_obj;
//         rlr_obj.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir, avgTotalControlStats);
//     }
// }

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZW_Stats& avgTotalLZWStats) {

//     for (const auto& file : files) {
//         std::cout << "LZW: " << file << "\n";
//         std::filesystem::path currentDir = file.parent_path();
//         std::string encodedFilename =  std::string(file.path()).erase(std::string(file).size()-7,6) + ".run_length_encoded_bin";
//         std::string dencodedFilename = std::string(file).erase(std::string(file).size()-7,6)  + ".run_length_decoded_bin";
//         for (int i = 0; i < numIterations; ++i) {
//             std::ifstream inFile(filename, std::ios::binary);
//             if (!inFile) {
//                 std::cout << "Error" << "\n";
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
//         std::cout << "LZP: " << file << "\n";
//         std::filesystem::path currentDir = file.parent_path();
//         avgTotalLZPStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);
//     }
// }

// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Huffman_Stats& avgTotalHuffmanStats) {

//     for (const auto& file : files) {
//         std::cout << file << "\n";
//         std::filesystem::path currentDir = file.parent_path();
//         avgTotalHuffmanStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations, currentDir);
//     }
// }


// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Control_Stats& avgTotalControlStats) {

//     for (const auto& file : files) {
//         // std::cout << "Control: " << file << "\n";
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
//     //std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << "\n";

//     // Write the encoded and decoded data to files
//     rlr_obj.writeEncodedFile(encoded, encodedFilename);
//     rlr_obj.writeDecodedFile(decoded, decodedFilename);
// }

void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files_vec, const int numIterations, RLR& rlr_obj) {
    //assert that the files are in the same directory
    assert(std::filesystem::equivalent(files_vec[0].parent_path(), files_vec[1].parent_path()));

    rlr_obj.Set_Data_Type_Size_And_Side_Resolutions(Get_Geometa_File_Path(files_vec[0].parent_path()));

    for (const auto& file : files_vec) {
        std::cerr << "File to be compressed: " << file << "\n";
        const std::filesystem::path parent_file_path = file.parent_path();
        std::string stem = file.stem().string();
        const std::filesystem::path encoded_file_path = parent_file_path/stem += ".rlr_encoded_bin";
        const std::filesystem::path decoded_file_path = parent_file_path/stem += ".rlr_decoded_bin";

        std::cerr << "Encoded File Path: " << encoded_file_path.c_str() << "\n";
        std::cerr << "Decoded File Path: " << decoded_file_path.c_str() << "\n";

        //this only works for single digit c numbers
        //I hate this
        // rewrite this code without using std::string
        auto extract_number_after = [&stem](const std::string& delimiter) {
            int pos = stem.find(delimiter);
            if (pos != std::string::npos) {
                return stem.substr(pos + delimiter.length(), 1);
            }
            return std::string{};
        };

        std::string side_str = extract_number_after("_s");
        std::string c_number_str = extract_number_after("_c");

        const char* side = side_str.c_str();
        uint8_t c_number = static_cast<uint8_t>(std::stoi(c_number_str));

        std::cerr << "Side: " << side << "\n";
        std::cerr << "C Number: " << static_cast<int>(c_number) << "\n";


        const int side_resolution = rlr_obj.Get_Side_Resolution(side, c_number);
        std::cerr << "Side Resolution: " << side_resolution << "\n";

        const int bytes_per_row = side_resolution * rlr_obj.Get_Data_Type_Size();
        const uint8_t num_rows = Get_File_Size_Bytes(file) / bytes_per_row;

        if(Get_File_Size_Bytes(file) % bytes_per_row != 0) {
            std::cerr << "ERROR: File size is not a multiple of the number of bytes per row." << "\n";
            std::cerr << "ERROR: File size: " << Get_File_Size_Bytes(file) << "\n";
            std::cerr << "ERROR: Bytes per row: " << std::to_string(bytes_per_row) << "\n";
            std::cerr << "ERROR: This probably means that rlr_obj.data_type_size is wrong for the file that is being commpressed" << "\n";
            std::cerr << "ERROR: You are trying to compress " << file.c_str() << "\n";
            std::cerr << "ERROR: Side Number is: " << side << "\n";
            std::cerr << "ERROR: C Number is: " << std::to_string(c_number) << "\n";
            std::cerr << "ERROR: Side Resolution is: " << std::to_string(side_resolution) << "\n";
            ERROR_MSG("ERROR:");
            //exit(1);
        }

        // // std::array<std::array<char, bytes_per_row>>, num_rows> partitioned_binary_data;

        // for(size_t i = 0; i < numIterations; i++){
        //     // std::ifstream inFile(file, std::ios::binary);
        //     // if (!inFile) {
        //     //     std::cout << "Error" << "\n";
        //     // }

        //     // inFile.seekg(0, std::ios::end);
        //     // size_t fileSize = inFile.tellg();
        //     // inFile.seekg(0, std::ios::beg);

        //     // std::vector<char> binaryData(fileSize);
        //     // inFile.read(binaryData.data(), fileSize);
        //     // inFile.close();

        //     // rlr_obj.Compute_Time_Encoded([&rlr_obj](std::vector<char> data){ rlr_obj.encode(data); }, binaryData);
        //     // rlr_obj.Compute_Time_Decoded([&rlr_obj](std::vector<char> data){ rlr_obj.decode(data); }, binaryData);

        //     // rlr_obj.writeEncodedFile(rlr_obj.getEncodedData(), encodedFilename.c_str());
        //     // rlr_obj.writeDecodedFile(rlr_obj.getDecodedData(), dencodedFilename.c_str());

        //     // rlr_obj.Compute_Compression_Ratio(file.c_str(), encodedFilename.c_str());
        //     // rlr_obj.Compute_File_Size(file.c_str());

        //     // for(size_t row = 0; row < num_rows; row++) {

        //     // }


        // }

    }
}
