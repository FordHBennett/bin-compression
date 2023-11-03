#pragma once

#include "common_stats.h"
#include "../functions/file_functions.h"



class RLR : public CommonStats {
    public:
        // Constructors
        RLR();
        RLR(const RLR& other);
        RLR& operator=(const RLR& other);
        ~RLR();

        void encode(const std::vector<char>& input);
        void decode(const std::vector<char>& input);

        const void writeEncodedFile(const std::vector<char>& encodedData, const char* filename);
        const void writeDecodedFile(const std::vector<char>& decodedData, const char* filename);

        //getters
        const char* getCompressionType() const {return compressionType;}
        const std::vector<char> getEncodedData() const {return encodedData;}
        const std::vector<char> getDecodedData() const {return decodedData;}



        //functions
        // void getFileStats(std::vector<char> &binaryData, const char* encodedFilename, const char* decodedFilename, size_t fileSize, std::filesystem::path& currentDir);
        // Control_Stats getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations, std::filesystem::path& currentDir, CommonStats &localStats);


    private:
        const char* compressionType = "RLR";
        std::vector<char> encodedData;
        std::vector<char> decodedData;


};