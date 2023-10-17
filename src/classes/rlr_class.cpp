#include "rlr_class.h"

//Constructors
RLR_Stats::RLR_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

RLR_Stats::RLR_Stats(double avgSizeBytes, double avgEncodedTimeMs, double avgDecodedTimeMs, double avgCompressionRatio, size_t avgPeakMemoryDuringEncoding, size_t avgPeakMemoryDuringDecoding, double avgEncodedThroughput, double avgThroughputDecoded) :
    avgSizeBytes(avgSizeBytes), avgEncodedTimeMs(avgEncodedTimeMs), avgDecodedTimeMs(avgDecodedTimeMs),
    avgCompressionRatio(avgCompressionRatio), avgPeakMemoryDuringEncoding(avgPeakMemoryDuringEncoding), avgPeakMemoryDuringDecoding(avgPeakMemoryDuringDecoding),
    avgEncodedThroughput(avgEncodedThroughput), avgThroughputDecoded(avgThroughputDecoded) {}

// Setters
void RLR_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void RLR_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void RLR_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void RLR_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void RLR_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void RLR_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void RLR_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void RLR_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double RLR_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double RLR_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double RLR_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double RLR_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t RLR_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t RLR_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double RLR_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double RLR_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }
