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
    calculateAvgStats(avgTotalRLRStats, totalFiles);
    calculateAvgStats(avgTotalLZWStats, totalFiles);
    calculateAvgStats(avgTotalLZPStats, totalFiles);
    
    // Print the average stats for all files
    printStats(avgTotalRLRStats);
    printStats(avgTotalLZWStats);
    printStats(avgTotalLZPStats);

    return 0;
}