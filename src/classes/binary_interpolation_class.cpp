#include "binary_interpolation_class.h"


union DoubleBits {
    double value;
    uint64_t bits;
};

// Constructors
Binary_Interpolation_Stats::Binary_Interpolation_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

Binary_Interpolation_Stats::Binary_Interpolation_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

std::vector<char> Binary_Interpolation_Stats::encode(const std::vector<char>& input) {
    double low = 0.0, high = 1.0;
    for (char bit : input) {
        double range = (high - low) * 0.5; // Calculating half the range
        if (bit == '0') {
            high = low + range;
        } else {
            low = low + range;
        }
    }

    // Convert the fractional part of 'low' to binary and store in a vector<char>
    std::vector<char> encoded;
    encoded.reserve(52);  // Pre-allocate memory

    DoubleBits db;
    db.value = low;
    uint64_t mask = 1ULL << 51;  // Start from the highest bit of the mantissa

    for (int i = 0; i < 52; ++i) {
        encoded.push_back((db.bits & mask) ? '1' : '0');
        mask >>= 1;
    }
    return encoded;
}


std::vector<char> Binary_Interpolation_Stats::decode(const std::vector<char>& encoded, int length) {
    std::vector<char> decoded;
    decoded.reserve(length);  // Reserving space for efficiency

    double low = 0.0, high = 1.0;
    double current = 0.0;
    double powerOfTwo = 0.5; // Start with the first bit after the decimal point

    // Convert encoded binary fraction to a double
    for (char bit : encoded) {
        if (bit == '1') {
            current += powerOfTwo;
        }
        powerOfTwo *= 0.5;
    }

    for (int i = 0; i < length; ++i) {
        double range = (high - low) * 0.5;  // Calculating half the range
        if (current < (low + range)) {
            decoded.push_back('0');
            high = low + range;
        } else {
            decoded.push_back('1');
            low = low + range;
        }
    }

    return decoded;
}


// Functions
void Binary_Interpolation_Stats::printStats() {
    std::cout << "Binary Interpolation: Average size (bytes): " << avgSizeBytes << std::endl;
    std::cout << "Binary Interpolation: Average encoded time (ms): " << avgEncodedTimeMs << std::endl;
    std::cout << "Binary Interpolation: Average decoded time (ms): " << avgDecodedTimeMs << std::endl;
    std::cout << "Binary Interpolation: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "Binary Interpolation: Average peak memory during encoding (bytes): " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "Binary Interpolation: Average peak memory during decoding (bytes): " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "Binary Interpolation: Average encoded throughput (bytes/s): " << avgEncodedThroughput << std::endl;
    std::cout << "Binary Interpolation: Average decoded throughput (bytes/s): " << avgThroughputDecoded << std::endl;
}

void Binary_Interpolation_Stats::calculateAvgStats(int divisor) {
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void Binary_Interpolation_Stats::getFileStats(std::vector<char>& binaryData, const char* binaryInterpolationEncodedFileName, const char* binaryInterpolationDencodedFileName, size_t fileSize) {
    // Encoding
    auto startEncoded = std::chrono::high_resolution_clock::now();
    std::vector<char> encoded = encode(binaryData);
    auto endEncoded = std::chrono::high_resolution_clock::now();
    auto encodedDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endEncoded - startEncoded);

    // Decoding
    auto startDecoded = std::chrono::high_resolution_clock::now();
    std::vector<char> decoded = decode(encoded, fileSize);
    auto endDecoded = std::chrono::high_resolution_clock::now();
    auto decodedDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endDecoded - startDecoded);

    // Calculate the throughput for encoding and decoding
    avgEncodedThroughput += binaryData.size() / static_cast<double>(encodedDuration.count()) * 1000; // bytes/sec
    avgThroughputDecoded += decoded.size() / static_cast<double>(decodedDuration.count()) * 1000; // bytes/sec


    // Verify that no data is lost by comparing decoded data with the original data
    bool dataMatches = binaryData == decoded;
    std::cout << "LZP Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

    // Create a binary file from LZP encoded data for further verification
    std::ofstream binaryInterpolationOutFile(binaryInterpolationEncodedFileName, std::ios::binary);
    std::ofstream binaryInterpolationDecodedOutFile(binaryInterpolationDencodedFileName, std::ios::binary);
    if (!binaryInterpolationOutFile || !binaryInterpolationDecodedOutFile) {
        std::cerr << "Error: Unable to create the LZP encoded or decoded file." << std::endl;
    }

    for (size_t i = 0; i < encoded.size(); ++i) {
        int code = encoded[i];
        binaryInterpolationOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
    }

    for (size_t i = 0; i < decoded.size(); ++i) {
        char byte = decoded[i];
        binaryInterpolationDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    binaryInterpolationOutFile.close();
    binaryInterpolationDecodedOutFile.close();

    // open the LZP encoded file and determine the file size
    avgSizeBytes += getFileSize(binaryInterpolationEncodedFileName);
    avgEncodedTimeMs += encodedDuration.count();
    avgDecodedTimeMs += decodedDuration.count();
    avgCompressionRatio += static_cast<double>(getFileSize(binaryInterpolationEncodedFileName)) / fileSize;
}

void Binary_Interpolation_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations) {
    // Read the binary file
    std::vector<char> binaryData;
    std::ifstream binaryFile(filename, std::ios::binary);
    if (!binaryFile) {
        std::cerr << "Error: Unable to open the binary file." << std::endl;
    }

    // Determine the file size
    size_t fileSize = getFileSize(filename);

    // Read the binary file
    binaryData.resize(fileSize);
    binaryFile.read(binaryData.data(), fileSize);
    binaryFile.close();

    // Perform encoding and decoding
    for (int i = 0; i < numIterations; ++i) {
        getFileStats(binaryData, "binary_interpolation_encoded.bin", "binary_interpolation_decoded.bin", fileSize);
    }
}

// Setters
void Binary_Interpolation_Stats::setAvgSizeBytes(double value) {
    avgSizeBytes = value;
}

void Binary_Interpolation_Stats::setAvgEncodedTimeMs(double value) {
    avgEncodedTimeMs = value;
}

void Binary_Interpolation_Stats::setAvgDecodedTimeMs(double value) {
    avgDecodedTimeMs = value;
}

void Binary_Interpolation_Stats::setAvgCompressionRatio(double value) {
    avgCompressionRatio = value;
}

void Binary_Interpolation_Stats::setAvgPeakMemoryDuringEncoding(size_t value) {
    avgPeakMemoryDuringEncoding = value;
}

void Binary_Interpolation_Stats::setAvgPeakMemoryDuringDecoding(size_t value) {
    avgPeakMemoryDuringDecoding = value;
}

void Binary_Interpolation_Stats::setAvgEncodedThroughput(double value) {
    avgEncodedThroughput = value;
}

void Binary_Interpolation_Stats::setAvgThroughputDecoded(double value) {
    avgThroughputDecoded = value;
}

// Getters
double Binary_Interpolation_Stats::getAvgSizeBytes() const {
    return avgSizeBytes;
}

double Binary_Interpolation_Stats::getAvgEncodedTimeMs() const {
    return avgEncodedTimeMs;
}

double Binary_Interpolation_Stats::getAvgDecodedTimeMs() const {
    return avgDecodedTimeMs;
}

double Binary_Interpolation_Stats::getAvgCompressionRatio() const {
    return avgCompressionRatio;
}

size_t Binary_Interpolation_Stats::getAvgPeakMemoryDuringEncoding() const {
    return avgPeakMemoryDuringEncoding;
}

size_t Binary_Interpolation_Stats::getAvgPeakMemoryDuringDecoding() const {
    return avgPeakMemoryDuringDecoding;
}

double Binary_Interpolation_Stats::getAvgEncodedThroughput() const {
    return avgEncodedThroughput;
}

double Binary_Interpolation_Stats::getAvgThroughputDecoded() const {
    return avgThroughputDecoded;
}
