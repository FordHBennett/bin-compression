#pragma once

#include <iostream>
#include <vector>
#include <cstddef>

#include "../classes/rlr_class.h"
#include "../classes/lzw_class.h"
#include "../classes/lzp_class.h"

// Forward declarations
class RLR_Stats;
class LZW_Stats;
class LZP_Stats;


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
