#include <iostream>
#include <fstream>
#include <random>
#include <thread>

#include "classes/rlr_class.h"
#include "classes/lzw_class.h"
#include "classes/lzp_class.h"
#include "classes/control_class.h"


#include "functions/file_functions.h"

int main() {
    const int numIterations = 5;

    RLR_Stats avgTotalRLRStats;
    LZW_Stats avgTotalLZWStats;
    LZP_Stats avgTotalLZPStats;
    Control_Stats avgTotalControlStats;

    std::vector<std::filesystem::path> geobinFiles;

    std::vector<std::pair<const char*, int>> geometa_dir_row_length;
    //Write a function to populate geometa_dir_row_length with the directory name and the number of rows in the directory
    // by reading .geometa files in the directory

    //FIX THE GETPEAKMEMORYUSAGE FUNCTION



    int totalFiles = getNumberOfGeobinFilesRecursivelyInDir("PlanetData/Earth/local/Grand_Canyon");
    std::filesystem::path planetDataPath = std::filesystem::path("PlanetData/Earth/local/Grand_Canyon");

    // Get list of all geobin files.
    for (const auto& entry : std::filesystem::recursive_directory_iterator(planetDataPath)) {
        if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
            geobinFiles.push_back(entry.path());
        }
    }

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int filesPerThread = geobinFiles.size() / numThreads;
    // Shared counter to keep track of the number of files processed
    std::atomic<int> filesProcessed(0);
    for (int t = 0; t < numThreads; ++t) {
        int start = t * filesPerThread;
        int end = (t == numThreads - 1) ? geobinFiles.size() : start + filesPerThread;

        threads.emplace_back([&filesProcessed, &avgTotalRLRStats, &avgTotalLZWStats, &avgTotalLZPStats, &avgTotalControlStats, &geobinFiles, start, end]() {
            for (int i = start; i < end; ++i) {
                processFiles(geobinFiles, numIterations, std::ref(avgTotalRLRStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalLZWStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalLZPStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalControlStats));

                // Increment the shared counter and print the % done
                int processed = filesProcessed.fetch_add(1) + 1;  // Fetch the current value and then add 1
                float percentageDone = (static_cast<float>(processed) / geobinFiles.size()) * 100;
                std::cout << "Processed: " << processed << " out of " << geobinFiles.size() << " (" << percentageDone << "% done)\n";
            }
        });
    }


    // Wait for all threads to finish.
    for (auto& t : threads) {
        t.join();
    }

    // Calculate the average stats for all files
    avgTotalRLRStats.calculateAvgStats(totalFiles);
    avgTotalLZWStats.calculateAvgStats(totalFiles);
    avgTotalLZPStats.calculateAvgStats(totalFiles);
    avgTotalControlStats.calculateAvgStats(totalFiles);

    // Print the average stats for all files
    avgTotalRLRStats.printStats();
    avgTotalLZWStats.printStats();
    avgTotalLZPStats.printStats();
    avgTotalControlStats.printStats();

    return 0;
}