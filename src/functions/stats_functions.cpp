#include "stats_functions.h"



void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations,
                                            RLR_Stats &avgTotalRLRStats, LZW_Stats &avgTotalLZWStats,
                                            LZP_Stats &avgTotalLZPStats)
{
    std::cout << "Compressing " << filename << "..." << std::endl;
    const char* runLengthFilename = "run_length_encoded.bin";
    const char* runLengthDecodedFilename = "run_length_decoded.bin";
    const char* lzwEncodedFilename = "lzw_encoded.bin";
    const char* lzwDecodedFilename = "lzw_decoded.bin";
    const char* lzpEncodedFileName = "lzp_encoded.bin";
    const char* lzpDecodedFileName = "lzp_decoded.bin";

    RLR_Stats avgRLRStats;
    LZW_Stats avgLZWStats;
    LZP_Stats avgLZPStats;

    for (int i = 0; i < numIterations; ++i) {
        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << std::endl;
        }

        inFile.seekg(0, std::ios::end);
        size_t fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<char> binaryData(fileSize);
        inFile.read(binaryData.data(), fileSize);
        inFile.close();

        // Perform run-length encoding and decoding
        avgRLRStats.getFileStats(binaryData, runLengthFilename, runLengthDecodedFilename, fileSize);

        // Perform LZW encoding and decoding
        avgLZWStats.getFileStats(binaryData, lzwEncodedFilename, lzwDecodedFilename, fileSize);

        // Perform LZP encoding and decoding
        avgLZPStats.getFileStats(binaryData, lzpEncodedFileName, lzpDecodedFileName, fileSize);

    }

    // Calculate the average stats for the current file
    avgRLRStats.calculateAvgStats(numIterations);
    avgLZWStats.calculateAvgStats(numIterations);
    avgLZPStats.calculateAvgStats(numIterations);

    avgTotalRLRStats.setAvgSizeBytes(avgTotalRLRStats.getAvgSizeBytes() + avgRLRStats.getAvgSizeBytes());
    avgTotalRLRStats.setAvgEncodedTimeMs(avgTotalRLRStats.getAvgEncodedTimeMs() + avgRLRStats.getAvgEncodedTimeMs());
    avgTotalRLRStats.setAvgDecodedTimeMs(avgTotalRLRStats.getAvgDecodedTimeMs() + avgRLRStats.getAvgDecodedTimeMs());
    avgTotalRLRStats.setAvgCompressionRatio(avgTotalRLRStats.getAvgCompressionRatio() + avgRLRStats.getAvgCompressionRatio());
    avgTotalRLRStats.setAvgPeakMemoryDuringEncoding(avgTotalRLRStats.getAvgPeakMemoryDuringEncoding() + avgRLRStats.getAvgPeakMemoryDuringEncoding());
    avgTotalRLRStats.setAvgPeakMemoryDuringDecoding(avgTotalRLRStats.getAvgPeakMemoryDuringDecoding() + avgRLRStats.getAvgPeakMemoryDuringDecoding());
    avgTotalRLRStats.setAvgEncodedThroughput(avgTotalRLRStats.getAvgEncodedThroughput() + avgRLRStats.getAvgEncodedThroughput());
    avgTotalRLRStats.setAvgThroughputDecoded(avgTotalRLRStats.getAvgThroughputDecoded() + avgRLRStats.getAvgThroughputDecoded());

    avgTotalLZWStats.setAvgSizeBytes(avgTotalLZWStats.getAvgSizeBytes() + avgLZWStats.getAvgSizeBytes());
    avgTotalLZWStats.setAvgEncodedTimeMs(avgTotalLZWStats.getAvgEncodedTimeMs() + avgLZWStats.getAvgEncodedTimeMs());
    avgTotalLZWStats.setAvgDecodedTimeMs(avgTotalLZWStats.getAvgDecodedTimeMs() + avgLZWStats.getAvgDecodedTimeMs());
    avgTotalLZWStats.setAvgCompressionRatio(avgTotalLZWStats.getAvgCompressionRatio() + avgLZWStats.getAvgCompressionRatio());
    avgTotalLZWStats.setAvgPeakMemoryDuringEncoding(avgTotalLZWStats.getAvgPeakMemoryDuringEncoding() + avgLZWStats.getAvgPeakMemoryDuringEncoding());
    avgTotalLZWStats.setAvgPeakMemoryDuringDecoding(avgTotalLZWStats.getAvgPeakMemoryDuringDecoding() + avgLZWStats.getAvgPeakMemoryDuringDecoding());
    avgTotalLZWStats.setAvgEncodedThroughput(avgTotalLZWStats.getAvgEncodedThroughput() + avgLZWStats.getAvgEncodedThroughput());
    avgTotalLZWStats.setAvgThroughputDecoded(avgTotalLZWStats.getAvgThroughputDecoded() + avgLZWStats.getAvgThroughputDecoded());

    avgTotalLZPStats.setAvgSizeBytes(avgTotalLZPStats.getAvgSizeBytes() + avgLZPStats.getAvgSizeBytes());
    avgTotalLZPStats.setAvgEncodedTimeMs(avgTotalLZPStats.getAvgEncodedTimeMs() + avgLZPStats.getAvgEncodedTimeMs());
    avgTotalLZPStats.setAvgDecodedTimeMs(avgTotalLZPStats.getAvgDecodedTimeMs() + avgLZPStats.getAvgDecodedTimeMs());
    avgTotalLZPStats.setAvgCompressionRatio(avgTotalLZPStats.getAvgCompressionRatio() + avgLZPStats.getAvgCompressionRatio());
    avgTotalLZPStats.setAvgPeakMemoryDuringEncoding(avgTotalLZPStats.getAvgPeakMemoryDuringEncoding() + avgLZPStats.getAvgPeakMemoryDuringEncoding());
    avgTotalLZPStats.setAvgPeakMemoryDuringDecoding(avgTotalLZPStats.getAvgPeakMemoryDuringDecoding() + avgLZPStats.getAvgPeakMemoryDuringDecoding());
    avgTotalLZPStats.setAvgEncodedThroughput(avgTotalLZPStats.getAvgEncodedThroughput() + avgLZPStats.getAvgEncodedThroughput());
    avgTotalLZPStats.setAvgThroughputDecoded(avgTotalLZPStats.getAvgThroughputDecoded() + avgLZPStats.getAvgThroughputDecoded());
}
