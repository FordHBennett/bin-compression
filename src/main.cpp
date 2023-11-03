#include <iostream>


// #include <fstream>
// #include <random>
// #include <thread>


#include "classes/common_stats.h"
#include "classes/rlr_class.h"
// #include "classes/lzw_class.h"
// #include "classes/lzp_class.h"
// #include "classes/control_class.h"


#include "functions/file_functions.h"

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

    std::filesystem::path grand_canyon_path = std::filesystem::current_path()/std::filesystem::path("PlanetData/Earth/local/Grand Canyon");


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
                    std::cerr << "Error accessing file: " << entry.path() << "\n"
                            << e.what() << "\n";
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // Handle errors during the directory iteration.
            std::cerr << "Error iterating through directory: " << grand_canyon_path << "\n"
                    << e.what() << "\n";
        }
    } else {
        std::cerr << "The path does not exist or is not a directory: " << grand_canyon_path << "\n";
    }

    std::cout << "You have successfully loaded " << geobin_files_vec.size() << " files.\n";

    Run_RLR_Compression_Decompression_On_Files(geobin_files_vec, numIterations, rlr);

    // const int numThreads = std::thread::hardware_concurrency();
    // // const int numThreads = 1;
    // std::vector<std::thread> threads;
    // int filesPerThread = geobin_files_vec.size() / numThreads;
    // // Shared counter to keep track of the number of files processed
    // std::atomic<int> filesProcessed(0);
    // for (int t = 0; t < numThreads; ++t) {
    //     int start = t * filesPerThread;
    //     int end = (t == numThreads - 1) ? geobin_files_vec.size() : start + filesPerThread;
    //     // partition geobin_files_vec into numThreads parts
    //     std::vector<std::filesystem::path> geobin_files_vecPartition(geobin_files_vec.begin() + start, geobin_files_vec.begin() + end);

    //     // threads.emplace_back([&filesProcessed, &avgTotalRLRStats, &avgTotalLZWStats, &avgTotalLZPStats, &avgTotalControlStats, &geobin_files_vecPartition, start, end]() {
    //     threads.emplace_back([&, geobin_files_vecPartition]() {

    //         // processFiles(geobin_files_vecPartition, numIterations, std::ref(avgTotalRLRStats));
    //         processFiles(geobin_files_vec, numIterations, std::ref(avgTotalLZWStats));
    //         // processFiles(geobin_files_vec, numIterations, std::ref(avgTotalLZPStats));
    //         // processFiles(geobin_files_vecPartition, numIterations, std::ref(avgTotalControlStats));

    //         // Increment the shared counter and print the % done
    //         int processed = filesProcessed.fetch_add(1) + 1;  // Fetch the current value and then add 1
    //         float percentageDone = (static_cast<float>(processed) / geobin_files_vec.size()) * 100;
    //         // std::cout << "Processed: " << processed << " out of " << geobin_files_vec.size() << " (" << percentageDone << "% done)\n";
    //     });
    // }


    // // Wait for all threads to finish.
    // for (auto& t : threads) {
    //     t.join();
    // }

    // // Calculate the average stats for all files
    // // avgTotalRLRStats.Calculate_Cumulative_Average_Stats_For_Directory(totalFiles);
    // avgTotalLZWStats.Calculate_Cumulative_Average_Stats_For_Directory(totalFiles);
    // // avgTotalLZPStats.Calculate_Cumulative_Average_Stats_For_Directory(totalFiles);
    // // avgTotalControlStats.Calculate_Cumulative_Average_Stats_For_Directory(totalFiles);

    // // Print the average stats for all files
    // // avgTotalRLRStats.Print_Stats();
    // avgTotalLZWStats.Print_Stats();
    // // avgTotalLZPStats.Print_Stats();
    // // avgTotalControlStats.Print_Stats();

    return 0;
}