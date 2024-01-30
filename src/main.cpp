#include <iostream>


// #include <fstream>
// #include <random>
// #include <thread>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
// #include <lzwfile.h>

#include "classes/common_stats.hpp"
#include "classes/rlr_class.hpp"
#include "classes/shannon_fano.hpp"
// #include "classes/lzw_class.hpp"
// #include "classes/lzw_class.h"
// #include "classes/lzp_class.h"
// #include "classes/control_class.h"

#include "functions/file_functions.hpp"

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

// pass a ref of an instance of common stats class to the processFiles function
// create an instance of the compression class in the processFiles function
// call the encode and decode functions of the compression class
// pass the encoded and decoded data to the computeFileStats function
// compute the stats and store them in the common stats class

//create a common stats class that has all the stats and then pass it to the processFiles function
int main() {
    // RLR rlr;
    // rlr.Set_Number_Of_Iterations(1);

    // const std::vector<std::filesystem::path> geometa_and_geobin_dir_path_vec = Get_Geobin_And_Geometa_Directory_Path_Vec(std::filesystem::path("PlanetData"));
    // for(int i = 0; i < geometa_and_geobin_dir_path_vec.size(); i++){
    //     std::vector<std::filesystem::path> geobin_files_vec = Get_Geobin_File_Vec(geometa_and_geobin_dir_path_vec[i]);
    //     Run_RLR_Compression_Decompression_On_Files(geobin_files_vec, rlr);

    //     rlr.Calculate_Cumulative_Average_Stats_For_Directory(geobin_files_vec.size());
    //     rlr.Compute_Encoded_Throughput();
    //     rlr.Compute_Decoded_Throughput();
    //     rlr.Write_Stats_To_File(std::filesystem::path{std::string{rlr.Get_Compression_Type()} + std::string{"_stats"}} /
    //                             std::filesystem::path{Remove_all_Seperators_From_Path(geometa_and_geobin_dir_path_vec[i]).string() +
    //                             std::string{"_stats.json"}}, rlr.Get_Compression_Type(), geometa_and_geobin_dir_path_vec[i].string());
    //     rlr.Reset_Stats();
    // }

    ShannonFano shannon_fano;
    shannon_fano.Set_Number_Of_Iterations(1);
    const std::vector<std::filesystem::path> geometa_and_geobin_dir_path_vec = Get_Geobin_And_Geometa_Directory_Path_Vec(std::filesystem::path("PlanetData"));
    for(int i = 0; i < geometa_and_geobin_dir_path_vec.size(); i++){
        std::vector<std::filesystem::path> geobin_files_vec = Get_Geobin_File_Vec(geometa_and_geobin_dir_path_vec[i]);
        Write_Shannon_Fano_Frequencies_To_Files(geobin_files_vec, shannon_fano);
        // Write_Shannon_Fano_Lookup_Table_To_Files(geobin_files_vec, shannon_fano);
    }


    return 0;
}