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
    avgTotalRLRStats.setAvgSizeBytes(avgTotalRLRStats.getAvgSizeBytes() / totalFiles);
    avgTotalRLRStats.setAvgEncodedTimeMs(avgTotalRLRStats.getAvgEncodedTimeMs() / totalFiles);
    avgTotalRLRStats.setAvgDecodedTimeMs(avgTotalRLRStats.getAvgDecodedTimeMs() / totalFiles);
    avgTotalRLRStats.setAvgCompressionRatio(avgTotalRLRStats.getAvgCompressionRatio() / totalFiles);
    avgTotalRLRStats.setAvgPeakMemoryDuringEncoding(avgTotalRLRStats.getAvgPeakMemoryDuringEncoding() / totalFiles);
    avgTotalRLRStats.setAvgPeakMemoryDuringDecoding(avgTotalRLRStats.getAvgPeakMemoryDuringDecoding() / totalFiles);
    avgTotalRLRStats.setAvgEncodedThroughput(avgTotalRLRStats.getAvgEncodedThroughput() / totalFiles);
    avgTotalRLRStats.setAvgThroughputDecoded(avgTotalRLRStats.getAvgThroughputDecoded() / totalFiles);

    avgTotalLZWStats.setAvgSizeBytes(avgTotalLZWStats.getAvgSizeBytes() / totalFiles);
    avgTotalLZWStats.setAvgEncodedTimeMs(avgTotalLZWStats.getAvgEncodedTimeMs() / totalFiles);
    avgTotalLZWStats.setAvgDecodedTimeMs(avgTotalLZWStats.getAvgDecodedTimeMs() / totalFiles);
    avgTotalLZWStats.setAvgCompressionRatio(avgTotalLZWStats.getAvgCompressionRatio() / totalFiles);
    avgTotalLZWStats.setAvgPeakMemoryDuringEncoding(avgTotalLZWStats.getAvgPeakMemoryDuringEncoding() / totalFiles);
    avgTotalLZWStats.setAvgPeakMemoryDuringDecoding(avgTotalLZWStats.getAvgPeakMemoryDuringDecoding() / totalFiles);
    avgTotalLZWStats.setAvgEncodedThroughput(avgTotalLZWStats.getAvgEncodedThroughput() / totalFiles);
    avgTotalLZWStats.setAvgThroughputDecoded(avgTotalLZWStats.getAvgThroughputDecoded() / totalFiles);

    avgTotalLZPStats.setAvgSizeBytes(avgTotalLZPStats.getAvgSizeBytes() / totalFiles);
    avgTotalLZPStats.setAvgEncodedTimeMs(avgTotalLZPStats.getAvgEncodedTimeMs() / totalFiles);
    avgTotalLZPStats.setAvgDecodedTimeMs(avgTotalLZPStats.getAvgDecodedTimeMs() / totalFiles);
    avgTotalLZPStats.setAvgCompressionRatio(avgTotalLZPStats.getAvgCompressionRatio() / totalFiles);
    avgTotalLZPStats.setAvgPeakMemoryDuringEncoding(avgTotalLZPStats.getAvgPeakMemoryDuringEncoding() / totalFiles);
    avgTotalLZPStats.setAvgPeakMemoryDuringDecoding(avgTotalLZPStats.getAvgPeakMemoryDuringDecoding() / totalFiles);
    avgTotalLZPStats.setAvgEncodedThroughput(avgTotalLZPStats.getAvgEncodedThroughput() / totalFiles);
    avgTotalLZPStats.setAvgThroughputDecoded(avgTotalLZPStats.getAvgThroughputDecoded() / totalFiles);

    // Print the average stats for all files
    printStats(avgTotalRLRStats);
    printStats(avgTotalLZWStats);
    printStats(avgTotalLZPStats);

    return 0;
}