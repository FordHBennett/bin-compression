#include "file_functions.h"


void generateRandomBinFile(const char* filename, long long fileSize, double zeroProbability) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
        return;
    }

    // Determine the number of zero bytes to write
    long long zeroBytes = static_cast<long long>(zeroProbability * fileSize);

    for (long long i = 0; i < zeroBytes; ++i) {
        char byte = 0;  // Write zero bytes
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    // Determine the number of non-zero bytes to write
    long long nonZeroBytes = fileSize - zeroBytes;

    std::default_random_engine generator;
    std::bernoulli_distribution distribution(0.5); // 50% probability for non-zero bits

    for (long long i = 0; i < nonZeroBytes; ++i) {
        bool bit = distribution(generator);
        char byte = static_cast<char>(bit);
        outFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
    }

    outFile.close();
}


double getFileSize(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
        return 1;
    }

    // Seek to the end to get the file size
    file.seekg(0, std::ios::end);
    double fileSize = file.tellg();

    // Close the file
    file.close();

    if (fileSize < 0) {
        std::cerr << "Error: Unable to determine the file size." << std::endl;
        return 1;
    }
    return fileSize;
}

// Function to return the number of Geobin files in a directory
int getNumberOfGeobinFilesRecursivelyInDir(const char* dir_name) {
    try {
        std::filesystem::path p(dir_name);

        if (!std::filesystem::exists(p)) {
            std::cerr << "Error: Directory does not exist: " << dir_name << std::endl;
            return -1;
        }

        int count = 0;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(p)) {
            if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".geobin") {
                count++;
            }
        }

        return count;
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}


void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, RLR_Stats& avgTotalRLRStats) {

    for (const auto& file : files) {
        //std::cout << file << std::endl;
        avgTotalRLRStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations);
    }
}

void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZW_Stats& avgTotalLZWStats) {

    for (const auto& file : files) {
        //std::cout << file << std::endl;
        avgTotalLZWStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations);
    }
}

void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZP_Stats& avgTotalLZPStats) {

    for (const auto& file : files) {
        //std::cout << file << std::endl;
        avgTotalLZPStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations);
    }
}

void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, Huffman_Stats& avgTotalHuffmanStats) {

    for (const auto& file : files) {
        //std::cout << file << std::endl;
        avgTotalHuffmanStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations);
    }
}

void processFiles(const std::vector<std::filesystem::path>& files, const int numIterations, LZO1_Stats& avgTotalBinaryInterpolationStats) {

    for (const auto& file : files) {
        //std::cout << file << std::endl;
        avgTotalBinaryInterpolationStats.getStatsFromEncodingDecodingFunctions(file.c_str(), numIterations);
    }
}
