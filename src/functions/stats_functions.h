#ifndef STATS_FUNCTIONS_H
#define STATS_FUNCTIONS_H

#include <iostream>
#include <vector>
#include <cstddef>

#include "file_functions.h"
#include "encoding_decoding_functions.h"

#include "../classes/rlr_class.h"
#include "../classes/lzw_class.h"
#include "../classes/lzp_class.h"

/**
 * The function `getRLRFileStats` performs run-length encoding and decoding on binary data, calculates
 * statistics such as encoding and decoding time, throughput, peak memory usage, and compression ratio,
 * and writes the encoded and decoded files.
 *
 * @param binaryData A vector of characters representing the binary data to be encoded and decoded
 * using run-length encoding.
 * @param runLengthFilename The name of the file where the run-length encoded data will be stored.
 * @param runLengthDecodedFilename The parameter `runLengthDecodedFilename` is a C-style string that
 * represents the name of the file where the run-length decoded data will be written.
 * @param fileSize The parameter `fileSize` represents the size of the original file in bytes.
 * @param avgRLRStats avgRLRStats is an object of type RLR_Stats, which is a user-defined class that
 * stores statistics related to run-length encoding and decoding. It contains the following member
 * variables:
 */
void getRLRFileStats(std::vector<char> &binaryData, const char* runLengthFilename, const char* runLengthDecodedFilename, size_t fileSize, RLR_Stats &avgRLRStats);

/**
 * The function `getLZWFileStats` performs LZW encoding and decoding on binary data, calculates various
 * statistics such as peak memory usage and throughput, verifies data integrity, and creates binary
 * files for further verification.
 *
 * @param binaryData A vector of characters representing the binary data to be encoded and decoded
 * using LZW algorithm.
 * @param lzwEncodedFilename The name of the file where the LZW encoded data will be stored.
 * @param lzwDecodedFilename The parameter `lzwDecodedFilename` is a string that represents the
 * filename of the LZW decoded file.
 * @param fileSize The parameter `fileSize` represents the size of the original binary data file in
 * bytes.
 * @param avgLZWStats avgLZWStats is an object of type LZW_Stats, which is a user-defined class that
 * stores statistics related to LZW encoding and decoding. It contains the following member variables:
 */
void getLZWFileStats(std::vector<char> &binaryData, const char* lzwEncodedFilename, const char* lzwDecodedFilename, size_t fileSize, LZW_Stats &avgLZWStats);

/**
 * The function `getLZPFileStat` encodes binary data using LZP compression, decodes it, and calculates
 * various statistics such as encoding and decoding time, throughput, compression ratio, and peak
 * memory usage.
 *
 * @param binaryData binaryData is a reference to a vector of characters that represents the input
 * binary data to be encoded and decoded using LZP compression.
 * @param lzpEncodedFileName The parameter `lzpEncodedFileName` is the name of the file where the LZP
 * encoded data will be stored.
 * @param lzpDecodedFileName The parameter `lzpDecodedFileName` is a pointer to a character array that
 * represents the name of the file where the LZP decoded data will be stored.
 * @param fileSize The parameter `fileSize` represents the size of the original file in bytes.
 * @param avgLZPStats avgLZPStats is an object of type LZP_Stats, which is used to store and calculate
 * various statistics related to the LZP encoding and decoding process. It contains the following
 * attributes:
 */
void getLZPFileStat(std::vector<char> &binaryData, const char* lzpEncodedFileName, const char* lzpDecodedFileName, size_t fileSize, LZP_Stats &avgLZPStats);

/**
 * The function calculates the average statistics of an RLR_Stats object by dividing each statistic by
 * a given divisor.
 *
 * @param rlr The parameter "rlr" is of type RLR_Stats, which is likely a class or struct that contains
 * various statistical measurements related to a data compression algorithm.
 * @param divisor The divisor is used to divide the average statistics values by a given number. It is
 * used to calculate the average statistics by dividing the sum of the statistics values by the number
 * of samples or iterations.
 */
void calculateAvgStats(RLR_Stats &rlr, int divisor);

/**
 * The function calculates the average statistics of an LZW_Stats object by dividing each statistic by
 * a given divisor.
 *
 * @param lzw The parameter "lzw" is of type lzw_Stats, which is likely a class or struct that contains
 * various statistical measurements related to a data compression algorithm.
 * @param divisor The divisor is used to divide the average statistics values by a given number. It is
 * used to calculate the average statistics by dividing the sum of the statistics values by the number
 * of samples or iterations.
 */
void calculateAvgStats(LZW_Stats &lzp, int divisor);

/**
 * The function calculates the average statistics of an LZP_Stats object by dividing each statistic by
 * a given divisor.
 *
 * @param lzp The parameter "lze" is of type lzp_Stats, which is likely a class or struct that contains
 * various statistical measurements related to a data compression algorithm.
 * @param divisor The divisor is used to divide the average statistics values by a given number. It is
 * used to calculate the average statistics by dividing the sum of the statistics values by the number
 * of samples or iterations.
 */
void calculateAvgStats(LZP_Stats &lzp, int divisor);

/**
 * Generate a random binary file with a given file size and a probability for zeros.
 *
 *  @param filename Name of the output file.
 *  @param numIterations Number of iterations to run the processing.
 *  @param avgTotalRLRStats Average RLR stats.
 *  @param avgTotalLZWStats Average LZW stats.
 *  @param avgTotalLZPStats Average LZP stats.
*/
void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations,
                                            RLR_Stats &avgTotalRLRStats, LZW_Stats &avgTotalLZWStats,
                                            LZP_Stats &avgTotalLZPStats);

/**
 * The function "printStats" prints out various statistics related to run-length encoding and decoding.
 *
 * @param avgTotalRLRStats The parameter `avgTotalRLRStats` is an object of type `RLR_Stats`. It is
 * used to store and retrieve various statistics related to run-length encoding.
 */
void printStats(RLR_Stats &avgTotalRLRStats);

/**
 * The function "printStats" prints out various statistics related to run-length encoding and decoding.
 *
 * @param avgTotalLZWStats The parameter `avgTotalLZWStats` is an object of type `LZW_Stats`. It is
 * used to store and retrieve various statistics related to the LZW (Run-Length Encoding) algorithm.
 * The `printStats` function takes this object as a parameter and uses its member functions to print
 */
void printStats(LZW_Stats &avgTotalLZWStats);

/**
 * The function "printStats" prints out various statistics related to run-length encoding and decoding.
 *
 * @param avgTotalLPWStats The parameter `avgTotalLPWStats` is of type `LZP_Stats`, which is a
 * user-defined class. It likely contains various statistics related to run-length encoding, such as
 * average file size, encoding time, decoding time, compression ratio, peak memory usage during
 * encoding and decoding,
 */
void printStats(LZP_Stats &avgTotalLZPStats);


#endif // STATS_FUNCTIONS_H