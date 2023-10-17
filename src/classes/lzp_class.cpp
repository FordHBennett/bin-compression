#include "lzp_class.h"

//Constructors
LZP_Stats::LZP_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

LZP_Stats::LZP_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

// Setters
void LZP_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void LZP_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void LZP_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void LZP_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void LZP_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void LZP_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void LZP_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void LZP_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double LZP_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double LZP_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double LZP_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double LZP_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t LZP_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t LZP_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double LZP_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double LZP_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }