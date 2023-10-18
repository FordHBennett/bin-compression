#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <random>

#include "../classes/rlr_class.h"
#include "../classes/lzw_class.h"
#include "../classes/lzp_class.h"

#include "../functions/stats_functions.h"

// Forward declarations
class RLR_Stats;
class LZW_Stats;
class LZP_Stats;



/**
 * Generate a random binary file with a given file size and a probability for zeros.
 * @param filename Name of the output file.
 * @param fileSize Size of the file to be generated.
 * @param zeroProbability Probability of having a zero byte in the file.
 */
void generateRandomBinFile(const char* filename, long long fileSize, double zeroProbability);

/**
 * Get the size of a file.
 * @param filename Name of the file.
 * @return Size of the file. Returns 1 in case of errors.
 */
double getFileSize(const char* filename);

/**
 * Get the number of .geobin files in a directory, recursively.
 * @param dir_name Name of the directory to search in.
 * @return Number of .geobin files. Returns -1 in case of errors.
 */
int getNumberOfGeobinFilesRecursivelyInDir(const char* dir_name);

/**
 * Process the given files and collect statistics on them.
 * @param files List of files to process.
 * @param numIterations Number of iterations to run the processing.
 * @param avgTotalRLRStats Average RLR stats.
 * @param avgTotalLZWStats Average LZW stats.
 * @param avgTotalLZPStats Average LZP stats.
 */
void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations,
                    RLR_Stats& avgTotalRLRStats, LZW_Stats& avgTotalLZWStats, LZP_Stats& avgTotalLZPStats);
