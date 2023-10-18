#include <iostream>
#include <fstream>
#include <random>
#include <thread>

#include "classes/rlr_class.h"
#include "classes/lzw_class.h"
#include "classes/lzp_class.h"

#include "functions/encoding_decoding_functions.h"
#include "functions/file_functions.h"
#include "functions/stats_functions.h"

std::mutex mtx;

int main() {
    const int numIterations = 10;

    RLR_Stats avgTotalRLRStats;
    LZW_Stats avgTotalLZWStats;
    LZP_Stats avgTotalLZPStats;

    std::vector<std::filesystem::path> geobinFiles;

    std::vector<std::pair<const char*, int>> geometa_dir_row_length;
    //Write a function to populate geometa_dir_row_length with the directory name and the number of rows in the directory
    // by reading .geometa files in the directory

    //FIX THE GETPEAKMEMORYUSAGE FUNCTION

    int totalFiles = getNumberOfGeobinFilesRecursivelyInDir("PlanetData");

    std::filesystem::path planetDataPath("PlanetData");

    // Get list of all geobin files.
    for (const auto& entry : std::filesystem::recursive_directory_iterator(planetDataPath)) {
        if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
            geobinFiles.push_back(entry.path());
        }
    }

    const int numThreads = std::thread::hardware_concurrency(); // Use the number of hardware threads available.
    std::vector<std::thread> threads(numThreads);
    int filesPerThread = geobinFiles.size() / numThreads;

    // Start threads.
    for (int i = 0; i < numThreads; ++i) {
        int start = i * filesPerThread;
        int end = (i == numThreads - 1) ? geobinFiles.size() : start + filesPerThread;

        threads[i] = std::thread(processFiles, std::vector<std::filesystem::path>(geobinFiles.begin() + start, geobinFiles.begin() + end),
                                 numIterations, std::ref(avgTotalRLRStats), std::ref(avgTotalLZWStats), std::ref(avgTotalLZPStats));
    }


    // Join threads.
    for (auto& t : threads) {
        t.join();
    }

    // Calculate the average stats for all files
    avgTotalRLRStats.calculateAvgStats(totalFiles);
    avgTotalLZWStats.calculateAvgStats(totalFiles);
    avgTotalLZPStats.calculateAvgStats(totalFiles);

    // Print the average stats for all files
    avgTotalRLRStats.printStats();
    avgTotalLZWStats.printStats();
    avgTotalLZPStats.printStats();

    return 0;
}