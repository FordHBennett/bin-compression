#pragma once

#include "../classes/common_stats.hpp"
#include <filesystem>
#include <unordered_map>


class RLR;
// class LZW_Stats;
// class LZP_Stats;
// class Huffman_Stats;
// class LZO1_Stats;
// class Control_Stats;



/**
 * Generate a random binary file with a given file size and a probability for zeros.
 * @param filename Name of the output file.
 * @param fileSize Size of the file to be generated.
 * @param zeroProbability Probability of having a zero byte in the file.
 */
void Generate_Random_Binary_File(const char* filename, long long fileSize, double zeroProbability);

/**
 * Get the size of a file.
 * @param filename Name of the file.
 * @return Size of the file. Returns 1 in case of errors.
 */
const uint64_t Get_File_Size_Bytes(const std::filesystem::path& file_path);

/**
 * Get the number of .geobin files in a directory, recursively.
 * @param dir_path Name of the directory to search in.
 * @return Number of .geobin files. Returns -1 in case of errors.
 */
const int Get_Number_Of_Geobin_Files_Recursively(const std::filesystem::path& dir_path);

const int Get_Number_Of_Geometa_Files(const std::filesystem::path& dir_path);

const std::filesystem::path  Get_Geometa_File_Path(const std::filesystem::path& dir_path);

/**
 * Process the given files and collect statistics on them.
 * @param files List of files to process.
 * @param numIterations Number of iterations to run the processing.
 * @param avgTotalRLRStats Average RLR stats.
 */
void Run_RLR_Compression_Decompression_On_Files(const std::vector<std::filesystem::path>& files, const int& numIterations, RLR& rlr_obj);
// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations,LZW_Stats& avgTotalLZWStats);
// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations,LZP_Stats& avgTotalLZPStats);
// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations,Huffman_Stats& avgTotalHuffmanStats);
// void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations,Control_Stats& avgTotalControlStats);

// void computeFileStats(std::vector<char> &binaryData, const std::file_system::path encodedFilename,
//                         const char* decodedFilename, size_t fileSize,
//                         std::filesystem::path& currentDir, CommonStats &localStats);
