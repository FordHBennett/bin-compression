#ifndef STATS_FUNCTIONS_H
#define STATS_FUNCTIONS_H

#include <iostream>

#include "file_functions.h"
#include "encoding_decoding_functions.h"

#include "../classes/rlr_class.h"
#include "../classes/lzw_class.h"
#include "../classes/lzp_class.h"

// Function to gather statistics from encoding and decoding processes
void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations,
                                            RLR_Stats &avgTotalRLRStats, LZW_Stats &avgTotalLZWStats, LZP_Stats &avgTotalLZPStats);

// Function to print statistics for Run-Length Encoding
void printStats(RLR_Stats &avgTotalRLRStats);

// Function to print statistics for LZW Encoding
void printStats(LZW_Stats &avgTotalLZWStats);

// Function to print statistics for LZP Encoding
void printStats(LZP_Stats &avgTotalLZPStats);


#endif // STATS_FUNCTIONS_H