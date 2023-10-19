#include <iostream>
#include <fstream>
#include <random>
#include <thread>

#include "classes/rlr_class.h"
#include "classes/lzw_class.h"
#include "classes/lzp_class.h"
#include "classes/huffman_class.h"
#include "classes/binary_interpolation_class.h"

#include "functions/file_functions.h"

std::mutex mtx;

int main() {
    const int numIterations = 10;

    RLR_Stats avgTotalRLRStats;
    LZW_Stats avgTotalLZWStats;
    LZP_Stats avgTotalLZPStats;
    Huffman_Stats avgTotalHuffmanStats;
    Binary_Interpolation_Stats avgTotalBinaryInterpolationStats;

    std::vector<std::filesystem::path> geobinFiles;

    std::vector<std::pair<const char*, int>> geometa_dir_row_length;
    //Write a function to populate geometa_dir_row_length with the directory name and the number of rows in the directory
    // by reading .geometa files in the directory

    //FIX THE GETPEAKMEMORYUSAGE FUNCTION

    // CREATE A BINARY INTERPOLATION CLASS
    // https://github.com/jermp/interpolative_coding

    // CREATE A ARITHMETIC CODING CLASS
    //  https://github.com/Abdelrahmanm22/DataCompression_Algorithms/blob/main/Arithmetic%20Coding(Binary)/Compression%20and%20DeCompression/main.py


    int totalFiles = getNumberOfGeobinFilesRecursivelyInDir("PlanetData");

    std::filesystem::path planetDataPath("PlanetData");

    // Get list of all geobin files.
    for (const auto& entry : std::filesystem::recursive_directory_iterator(planetDataPath)) {
        if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
            geobinFiles.push_back(entry.path());
        }
    }

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int filesPerThread = geobinFiles.size() / numThreads;
    for (int t = 0; t < numThreads; ++t) {
        int start = t * filesPerThread;
        int end = (t == numThreads - 1) ? geobinFiles.size() : start + filesPerThread;

        threads.emplace_back([=, &avgTotalRLRStats, &avgTotalLZWStats, &avgTotalLZPStats, &avgTotalHuffmanStats, &avgTotalBinaryInterpolationStats]() {
            for (int i = start; i < end; ++i) {
                processFiles(geobinFiles, numIterations, std::ref(avgTotalRLRStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalLZWStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalLZPStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalHuffmanStats));
                processFiles(geobinFiles, numIterations, std::ref(avgTotalBinaryInterpolationStats));
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

    // Print the average stats for all files
    avgTotalRLRStats.printStats();
    avgTotalLZWStats.printStats();
    avgTotalLZPStats.printStats();

    return 0;
}