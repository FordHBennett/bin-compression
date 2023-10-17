#include "stats_functions.h"

void getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations,
                                            RLR_Stats &avgTotalRLRStats, LZW_Stats &avgTotalLZWStats, LZP_Stats &avgTotalLZPStats)
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

        // Perform run-length encoding
        auto startEncodRunLength = std::chrono::high_resolution_clock::now();
        auto runLengthEncoded = runLengthEncode(binaryData);
        auto stopEncodRunLength = std::chrono::high_resolution_clock::now();
        auto durationEncodRunLength = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodRunLength - startEncodRunLength);

        // Perform run-length decoding
        auto startDecodRunLength = std::chrono::high_resolution_clock::now();
        auto runLengthDecoded = runLengthDecode(runLengthEncoded);
        auto stopDecodRunLength = std::chrono::high_resolution_clock::now();
        auto durationDecodRunLength = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodRunLength - startDecodRunLength);

        // Calculate the peak memory usage
        avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgRLRStats.setAvgEncodedThroughput(avgRLRStats.getAvgEncodedThroughput() + binaryData.size() / static_cast<double>(durationEncodRunLength.count()) * 1000); // bytes/sec
        avgRLRStats.setAvgThroughputDecoded(avgRLRStats.getAvgThroughputDecoded() + runLengthDecoded.size() / static_cast<double>(durationDecodRunLength.count()) * 1000); // bytes/sec

        // Verify that no data is lost by comparing decoded data with the original data
        bool dataMatches = binaryData == runLengthDecoded;
        std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

        // Write the run-length encoded and decoded files
        std::ofstream runLengthOutFile(runLengthFilename, std::ios::binary);
        std::ofstream runLengthDecodedOutFile(runLengthDecodedFilename, std::ios::binary);
        if (!runLengthOutFile || !runLengthDecodedOutFile) {
            std::cerr << "Error: Unable to create the run-length encoded or decoded file." << std::endl;
        }

        for (size_t i = 0; i < runLengthEncoded.size(); ++i) {
            char byte = static_cast<char>(runLengthEncoded[i].first);
            runLengthOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        for (size_t i = 0; i < runLengthDecoded.size(); ++i) {
            char byte = runLengthDecoded[i];
            runLengthDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        runLengthOutFile.close();
        runLengthDecodedOutFile.close();

        // open the run-length encoded file and determine the file size
        avgRLRStats.setAvgSizeBytes(avgRLRStats.getAvgSizeBytes() + getFileSize(runLengthFilename));
        avgRLRStats.setAvgEncodedTimeMs(avgRLRStats.getAvgEncodedTimeMs() + durationEncodRunLength.count());
        avgRLRStats.setAvgDecodedTimeMs(avgRLRStats.getAvgDecodedTimeMs() + durationDecodRunLength.count());
        avgRLRStats.setAvgCompressionRatio(avgRLRStats.getAvgCompressionRatio() + static_cast<double>(getFileSize(runLengthFilename)) / fileSize);


        // Perform LZW encoding
        auto startEncodeLzw = std::chrono::high_resolution_clock::now();
        std::vector<int> lzwEncoded = lzwEncode(binaryData);
        auto stopEncodeLzw = std::chrono::high_resolution_clock::now();
        auto durationEncodeLzw = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodeLzw - startEncodeLzw);

        // Perform LZW decoding
        auto startDecodeLzw = std::chrono::high_resolution_clock::now();
        std::vector<char> lzwDecoded = lzwDecode(lzwEncoded);
        auto stopDecodeLzw = std::chrono::high_resolution_clock::now();
        auto durationDecodeLzw = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodeLzw - startDecodeLzw);

        // Calculate the peak memory usage
        avgLZWStats.setAvgPeakMemoryDuringDecoding(avgLZWStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        avgLZWStats.setAvgPeakMemoryDuringEncoding(avgLZWStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgLZWStats.setAvgEncodedThroughput(avgLZWStats.getAvgEncodedThroughput() + binaryData.size() / static_cast<double>(durationEncodeLzw.count()) * 1000); // bytes/sec
        avgLZWStats.setAvgThroughputDecoded(avgLZWStats.getAvgThroughputDecoded() + lzwDecoded.size() / static_cast<double>(durationDecodeLzw.count()) * 1000); // bytes/sec

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzwDataMatches = binaryData == lzwDecoded;
        std::cout << "LZW Data Matches: " << (lzwDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZW encoded data for further verification
        std::ofstream lzwOutFile(lzwEncodedFilename, std::ios::binary);
        std::ofstream lzwDecodedOutFile(lzwDecodedFilename, std::ios::binary);
        if (!lzwOutFile || !lzwDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZW encoded or decoded file." << std::endl;
        }

        for (size_t i = 0; i < lzwEncoded.size(); ++i) {
            int code = lzwEncoded[i];
            lzwOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
        }

        for (size_t i = 0; i < lzwDecoded.size(); ++i) {
            char byte = lzwDecoded[i];
            lzwDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        lzwOutFile.close();
        lzwDecodedOutFile.close();

        // open the LZW encoded file and determine the file size
        avgLZWStats.setAvgSizeBytes(avgLZWStats.getAvgSizeBytes() + getFileSize(lzwEncodedFilename));
        avgLZWStats.setAvgEncodedTimeMs(avgLZWStats.getAvgEncodedTimeMs() + durationEncodeLzw.count());
        avgLZWStats.setAvgDecodedTimeMs(avgLZWStats.getAvgDecodedTimeMs() + durationDecodeLzw.count());
        avgLZWStats.setAvgCompressionRatio(avgLZWStats.getAvgCompressionRatio() + static_cast<double>(getFileSize(lzwEncodedFilename)) / fileSize);


        // Perform LZP encoding
        auto startEncodeLzp = std::chrono::high_resolution_clock::now();
        std::vector<int> lzpEncoded = lzpEncode(binaryData);
        auto stopEncodeLzp = std::chrono::high_resolution_clock::now();
        auto durationEncodeLzp = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodeLzp - startEncodeLzp);

        // Perform LZP decoding
        auto startDecodeLzp = std::chrono::high_resolution_clock::now();
        std::vector<char> lzpDecoded = lzpDecode(lzpEncoded);
        auto stopDecodeLzp = std::chrono::high_resolution_clock::now();
        auto durationDecodeLzp = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodeLzp - startDecodeLzp);

        // Calculate the peak memory usage
        avgLZPStats.setAvgPeakMemoryDuringDecoding(avgLZPStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        avgLZPStats.setAvgPeakMemoryDuringEncoding(avgLZPStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgLZPStats.setAvgEncodedThroughput(avgLZPStats.getAvgEncodedThroughput() + binaryData.size() / static_cast<double>(durationEncodeLzp.count()) * 1000); // bytes/sec
        avgLZPStats.setAvgThroughputDecoded(avgLZPStats.getAvgThroughputDecoded() + lzpDecoded.size() / static_cast<double>(durationDecodeLzp.count()) * 1000); // bytes/sec

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzpDataMatches = binaryData == lzpDecoded;
        std::cout << "LZP Data Matches: " << (lzpDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZP encoded data for further verification
        std::ofstream lzpOutFile(lzpEncodedFileName, std::ios::binary);
        std::ofstream lzpDecodedOutFile(lzpDecodedFileName, std::ios::binary);
        if (!lzpOutFile || !lzpDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZP encoded or decoded file." << std::endl;
        }

        for (size_t i = 0; i < lzpEncoded.size(); ++i) {
            int code = lzpEncoded[i];
            lzpOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
        }

        for (size_t i = 0; i < lzpDecoded.size(); ++i) {
            char byte = lzpDecoded[i];
            lzpDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        lzpOutFile.close();
        lzpDecodedOutFile.close();

        // open the LZP encoded file and determine the file size
        avgLZPStats.setAvgSizeBytes(avgLZPStats.getAvgSizeBytes() + getFileSize(lzpEncodedFileName));
        avgLZPStats.setAvgEncodedTimeMs(avgLZPStats.getAvgEncodedTimeMs() + durationEncodeLzp.count());
        avgLZPStats.setAvgDecodedTimeMs(avgLZPStats.getAvgDecodedTimeMs() + durationDecodeLzp.count());
        avgLZPStats.setAvgCompressionRatio(avgLZPStats.getAvgCompressionRatio() + static_cast<double>(getFileSize(lzpEncodedFileName)) / fileSize);
    }

    // Calculate the average stats for the current file
    avgRLRStats.setAvgSizeBytes(avgRLRStats.getAvgSizeBytes() / numIterations);
    avgRLRStats.setAvgEncodedTimeMs(avgRLRStats.getAvgEncodedTimeMs() / numIterations);
    avgRLRStats.setAvgDecodedTimeMs(avgRLRStats.getAvgDecodedTimeMs() / numIterations);
    avgRLRStats.setAvgCompressionRatio(avgRLRStats.getAvgCompressionRatio() / numIterations);
    avgRLRStats.setAvgPeakMemoryDuringEncoding(avgRLRStats.getAvgPeakMemoryDuringEncoding() / numIterations);
    avgRLRStats.setAvgPeakMemoryDuringDecoding(avgRLRStats.getAvgPeakMemoryDuringDecoding() / numIterations);
    avgRLRStats.setAvgEncodedThroughput(avgRLRStats.getAvgEncodedThroughput() / numIterations);
    avgRLRStats.setAvgThroughputDecoded(avgRLRStats.getAvgThroughputDecoded() / numIterations);

    avgLZWStats.setAvgSizeBytes(avgLZWStats.getAvgSizeBytes() / numIterations);
    avgLZWStats.setAvgEncodedTimeMs(avgLZWStats.getAvgEncodedTimeMs() / numIterations);
    avgLZWStats.setAvgDecodedTimeMs(avgLZWStats.getAvgDecodedTimeMs() / numIterations);
    avgLZWStats.setAvgCompressionRatio(avgLZWStats.getAvgCompressionRatio() / numIterations);
    avgLZWStats.setAvgPeakMemoryDuringEncoding(avgLZWStats.getAvgPeakMemoryDuringEncoding() / numIterations);
    avgLZWStats.setAvgPeakMemoryDuringDecoding(avgLZWStats.getAvgPeakMemoryDuringDecoding() / numIterations);
    avgLZWStats.setAvgEncodedThroughput(avgLZWStats.getAvgEncodedThroughput() / numIterations);
    avgLZWStats.setAvgThroughputDecoded(avgLZWStats.getAvgThroughputDecoded() / numIterations);

    avgLZPStats.setAvgSizeBytes(avgLZPStats.getAvgSizeBytes() / numIterations);
    avgLZPStats.setAvgEncodedTimeMs(avgLZPStats.getAvgEncodedTimeMs() / numIterations);
    avgLZPStats.setAvgDecodedTimeMs(avgLZPStats.getAvgDecodedTimeMs() / numIterations);
    avgLZPStats.setAvgCompressionRatio(avgLZPStats.getAvgCompressionRatio() / numIterations);
    avgLZPStats.setAvgPeakMemoryDuringEncoding(avgLZPStats.getAvgPeakMemoryDuringEncoding() / numIterations);
    avgLZPStats.setAvgPeakMemoryDuringDecoding(avgLZPStats.getAvgPeakMemoryDuringDecoding() / numIterations);
    avgLZPStats.setAvgEncodedThroughput(avgLZPStats.getAvgEncodedThroughput() / numIterations);
    avgLZPStats.setAvgThroughputDecoded(avgLZPStats.getAvgThroughputDecoded() / numIterations);

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

void printStats( RLR_Stats &avgTotalRLRStats){
    std::cout << "Average Run-Length Encoded File Size: " << avgTotalRLRStats.getAvgSizeBytes() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Time: " << avgTotalRLRStats.getAvgEncodedTimeMs() << " ms" << std::endl;
    std::cout << "Average Run-Length Decoding Time: " << avgTotalRLRStats.getAvgDecodedTimeMs() << " ms" << std::endl << std::endl;
    std::cout << "Average Run-Length Compression Ratio: " << avgTotalRLRStats.getAvgCompressionRatio() << std::endl;
    std::cout << "Average Run-Length Encoding Peak Memory Usage: " << avgTotalRLRStats.getAvgPeakMemoryDuringEncoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Decoding Peak Memory Usage: " << avgTotalRLRStats.getAvgPeakMemoryDuringDecoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Throughput: " << avgTotalRLRStats.getAvgEncodedThroughput() << " bytes/sec" << std::endl;
    std::cout << "Average Run-Length Decoding Throughput: " << avgTotalRLRStats.getAvgThroughputDecoded() << " bytes/sec" << std::endl << std::endl;
}

void printStats( LZW_Stats &avgTotalLZWStats){
    std::cout << "Average Run-Length Encoded File Size: " << avgTotalLZWStats.getAvgSizeBytes() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Time: " << avgTotalLZWStats.getAvgEncodedTimeMs() << " ms" << std::endl;
    std::cout << "Average Run-Length Decoding Time: " << avgTotalLZWStats.getAvgDecodedTimeMs() << " ms" << std::endl << std::endl;
    std::cout << "Average Run-Length Compression Ratio: " << avgTotalLZWStats.getAvgCompressionRatio() << std::endl;
    std::cout << "Average Run-Length Encoding Peak Memory Usage: " << avgTotalLZWStats.getAvgPeakMemoryDuringEncoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Decoding Peak Memory Usage: " << avgTotalLZWStats.getAvgPeakMemoryDuringDecoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Throughput: " << avgTotalLZWStats.getAvgEncodedThroughput() << " bytes/sec" << std::endl;
    std::cout << "Average Run-Length Decoding Throughput: " << avgTotalLZWStats.getAvgThroughputDecoded() << " bytes/sec" << std::endl << std::endl;
}

void printStats( LZP_Stats &avgTotalLPWStats){
    std::cout << "Average Run-Length Encoded File Size: " << avgTotalLPWStats.getAvgSizeBytes() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Time: " << avgTotalLPWStats.getAvgEncodedTimeMs() << " ms" << std::endl;
    std::cout << "Average Run-Length Decoding Time: " << avgTotalLPWStats.getAvgDecodedTimeMs() << " ms" << std::endl << std::endl;
    std::cout << "Average Run-Length Compression Ratio: " << avgTotalLPWStats.getAvgCompressionRatio() << std::endl;
    std::cout << "Average Run-Length Encoding Peak Memory Usage: " << avgTotalLPWStats.getAvgPeakMemoryDuringEncoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Decoding Peak Memory Usage: " << avgTotalLPWStats.getAvgPeakMemoryDuringDecoding() << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Throughput: " << avgTotalLPWStats.getAvgEncodedThroughput() << " bytes/sec" << std::endl;
    std::cout << "Average Run-Length Decoding Throughput: " << avgTotalLPWStats.getAvgThroughputDecoded() << " bytes/sec" << std::endl << std::endl;
}