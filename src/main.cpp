#include <iostream>


// #include <fstream>
// #include <random>
// #include <thread>
#include <fstream>
#include <iostream>
// #include <lz4file.h>

#include "classes/common_stats.hpp"
#include "classes/rlr_class.hpp"
// #include "classes/lz4_class.hpp"
// #include "classes/lzw_class.h"
// #include "classes/lzp_class.h"
// #include "classes/control_class.h"


#include "functions/file_functions.hpp"
#include "functions/debug_functions.hpp"

// pass a ref of an instance of common stats class to the processFiles function
// create an instance of the compression class in the processFiles function
// call the encode and decode functions of the compression class
// pass the encoded and decoded data to the computeFileStats function
// compute the stats and store them in the common stats class

//create a common stats class that has all the stats and then pass it to the processFiles function
int main() {
    const int numIterations = 1;

    RLR rlr;
    // LZW_Stats avgTotalLZWStats;
    // LZP_Stats avgTotalLZPStats;
    // Control_Stats avgTotalControlStats;

    std::vector<std::filesystem::path> geobin_files_vec;

    std::filesystem::path grand_canyon_path = std::filesystem::current_path()/std::filesystem::path("PlanetData/Jupiter/Europa");


    // Get list of all geobin files.
    if (std::filesystem::exists(grand_canyon_path) && std::filesystem::is_directory(grand_canyon_path)) {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(grand_canyon_path)) {
                try {
                    if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
                        geobin_files_vec.push_back(entry.path());
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    // Handle file-specific errors.
                    // std::cerr << "Error accessing file: " << entry.path() << '\n'
                    //         << e.what() << '\n';
                    ERROR_MSG_AND_EXIT(std::string{"Error accessing file: "} + entry.path().string() + '\n' + std::string{"ERROR CODE: "} + std::string{e.what()} );
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // Handle errors during the directory iteration.
            // std::cerr << "Error iterating through directory: " << grand_canyon_path << '\n'
            //         << e.what() << '\n';
            ERROR_MSG_AND_EXIT(std::string{"Error iterating through directory: "} + grand_canyon_path.string() + '\n' + std::string{"ERROR CODE: "} + std::string{e.what()} );
        }
    } else {
        // std::cerr << "The path does not exist or is not a directory: " << grand_canyon_path << '\n';
        ERROR_MSG_AND_EXIT(std::string{"The path does not exist or is not a directory: "} + grand_canyon_path.string());
    }

    Run_RLR_Compression_Decompression_On_Files(geobin_files_vec, numIterations, rlr);

    // LZ4 lz4;
    // char inpFilename[256] = { 0 };
    // char lz4Filename[256] = { 0 };
    // char decFilename[256] = { 0 };
    // char input_file_name[] = "PlanetData/Earth/local/Grand Canyon/gc_dem_s3_c3_lod6.geobin";
    // char lz4_file_name[] = "lz4_encoded.bin";
    // char decoded_file_name[] = "lz4_decoded.bin";
    // FILE* input_file_ptr = fopen(input_file_name, "rb");
    // FILE* lz4_file_ptr = fopen(lz4_file_name, "wb");
    // FILE* decoded_file_ptr = fopen(decoded_file_name, "wb");

    // //compress
    // LZ4F_errorCode_t ret = lz4.Compress_File(input_file_ptr, lz4_file_ptr);
    // fclose(input_file_ptr);
    // fclose(lz4_file_ptr);
    // if (ret) {
    //     std::cerr << "Error: LZ4 compression failed." << '\n';
    //     ERROR_MSG_AND_EXIT("LZ4 compression failed.");
    // }

    // //decompress
    // lz4_file_ptr = fopen(lz4_file_name, "rb");
    // ret = lz4.Decompress_File(lz4_file_ptr, decoded_file_ptr);
    // fclose(lz4_file_ptr);
    // fclose(decoded_file_ptr);

    // if (ret) {
    //     std::cerr << "Error: LZ4 decompression failed." << '\n';
    //     ERROR_MSG_AND_EXIT("LZ4 decompression failed.");
    // }

    // //verify populating the decoded data vector and original data vector
    // std::ifstream input_stream(input_file_name, std::ios::binary);
    // std::ifstream decoded_stream(decoded_file_name, std::ios::binary);
    // std::vector<char> original_data;
    // std::vector<char> decoded_data;
    // char c;
    // while(input_stream.get(c)){
    //     original_data.emplace_back(c);
    // }
    // while(decoded_stream.get(c)){
    //     decoded_data.emplace_back(c);
    // }
    // lz4.Is_Decoded_Data_Equal_To_Original_Data(original_data, decoded_data);

    // const size_t max_bytes = 1e9;
    // const size_t ring_buffer_bytes = 1e6;
    // std::ifstream input_stream("PlanetData/Earth/local/Grand Canyon/gc_dem_s3_c3_lod6.geobin", std::ios::binary);
    // lz4.encode(input_stream, max_bytes, ring_buffer_bytes);
    // lz4.Write_Compressed_File(lz4.Get_Encoded_Data(), "test.lz4");
    // lz4.decode(max_bytes, ring_buffer_bytes);
    // lz4.Write_Decompressed_File(lz4.Get_Decoded_Data(), "test2.geobin");




    return 0;
}