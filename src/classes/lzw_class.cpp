#include "lzw_class.h"

//Constructors
LZW_Stats::LZW_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

LZW_Stats::LZW_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

// Setters
void LZW_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void LZW_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void LZW_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void LZW_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void LZW_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void LZW_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void LZW_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void LZW_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double LZW_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double LZW_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double LZW_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double LZW_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t LZW_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t LZW_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double LZW_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double LZW_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }