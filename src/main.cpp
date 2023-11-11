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
    const int number_of_iteration = 1;

    RLR rlr;

    const std::vector<std::filesystem::path> geometa_and_geobin_dir_path_vec = Get_Geobin_And_Geometa_Directory_Path_Vec(std::filesystem::path("PlanetData/Mars"));
    for(int i = 0; i < geometa_and_geobin_dir_path_vec.size(); i++){
        std::vector<std::filesystem::path> geobin_files_vec = Get_Geobin_File_Vec(geometa_and_geobin_dir_path_vec[i]);
        Run_RLR_Compression_Decompression_On_Files(geobin_files_vec, number_of_iteration, rlr);
        rlr.Calculate_Cumulative_Average_Stats_For_Directory(number_of_iteration, geobin_files_vec.size());
        rlr.Compute_Encoded_Throughput();
        rlr.Compute_Decoded_Throughput();
        rlr.Write_Stats_To_File(std::filesystem::path{"stats"}/std::filesystem::path{Remove_all_Seperators_From_Path(geometa_and_geobin_dir_path_vec[i]).string() +
                                std::string{"_stats.json"}}, rlr.Get_Compression_Type(), geometa_and_geobin_dir_path_vec[i].string());
        rlr.Reset_Stats();
    }

    return 0;
}