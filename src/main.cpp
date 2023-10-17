#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <queue>
#include <map>
#include <algorithm>
#include <string>
#include <thread>
#include <list>
#include <unordered_map>
#include <stdexcept>


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

std::vector<std::pair<char, int>> runLengthEncode(const std::vector<char>& input) {
    if (input.empty()) {
        return {};
    }

    std::vector<std::pair<char, int>> runLengthEncoded;
    runLengthEncoded.reserve(input.size()); // Reserve the maximum potential size.

    char currentChar = input[0];
    int runLength = 1;

    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == currentChar && runLength < 255) {
            runLength++;
        } else {
            runLengthEncoded.emplace_back(currentChar, runLength);
            currentChar = input[i];
            runLength = 1;
        }
    }

    // After the loop, don't forget to add the last run.
    runLengthEncoded.emplace_back(currentChar, runLength);

    runLengthEncoded.shrink_to_fit(); // Reduce capacity to fit the actual size.

    return runLengthEncoded;
}

std::vector<char> runLengthDecode(const std::vector<std::pair<char, int>>& input) {
    std::vector<char> runLengthDecoded;
    size_t totalLength = 0;

    // Calculate the total length to reserve capacity.
    for (const auto& pair : input) {
        totalLength += pair.second;
    }
    runLengthDecoded.reserve(totalLength);

    for (const auto& pair : input) {
        std::fill_n(std::back_inserter(runLengthDecoded), pair.second, pair.first);
    }

    return runLengthDecoded;
}

// Function to perform LZW encoding
std::vector<int> lzwEncode(const std::vector<char>& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzwEncoded;
    lzwEncoded.reserve(input.size());  // Reduce potential reallocations

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            current.pop_back();  // Remove last character
            lzwEncoded.push_back(dictionary[current]);
            dictionary[current + c] = dictionary.size();
            current = c;
        }
    }

    if (!current.empty()) {
        lzwEncoded.push_back(dictionary[current]);
    }

    return lzwEncoded;
}

// Function to perform LZW decoding
std::vector<char> lzwDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzwDecoded;
    lzwDecoded.reserve(input.size());  // Reduce potential reallocations

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzwDecoded.insert(lzwDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZW decoding error: Invalid code.");
        }

        lzwDecoded.insert(lzwDecoded.end(), entry.begin(), entry.end());

        // Add a new entry to the dictionary
        dictionary[dictionary.size()] = current + entry[0];

        current = entry;
    }

    return lzwDecoded;
}

// Lempel-Ziv Parallel (LZP) encoding
std::vector<int> lzpEncode(const std::vector<char>& input) {
    std::unordered_map<std::string, int> dictionary;
    std::vector<int> lzpEncoded;
    lzpEncoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current = "";
    for (char c : input) {
        current += c;
        if (dictionary.find(current) == dictionary.end()) {
            current.pop_back();  // Remove the last character
            lzpEncoded.push_back(dictionary[current]);
            dictionary[current + c] = dictionary.size();
            current = c;
        }
    }

    if (!current.empty()) {
        lzpEncoded.push_back(dictionary[current]);
    }

    return lzpEncoded;
}

// Lempel-Ziv Parallel (LZP) decoding
std::vector<char> lzpDecode(const std::vector<int>& input) {
    if (input.empty()) {
        return {};
    }

    std::unordered_map<int, std::string> dictionary;
    std::vector<char> lzpDecoded;
    lzpDecoded.reserve(input.size());  // Pre-allocate memory

    // Initialize the dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary.at(input[0]);
    lzpDecoded.insert(lzpDecoded.end(), current.begin(), current.end());

    for (size_t i = 1; i < input.size(); ++i) {
        int code = input[i];
        std::string entry;

        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictionary.size()) {
            entry = current + current[0];
        } else {
            throw std::runtime_error("LZP decoding error: Invalid code.");
        }

        lzpDecoded.insert(lzpDecoded.end(), entry.begin(), entry.end());

        dictionary[dictionary.size()] = current + entry[0];
        current = entry;
    }

    return lzpDecoded;
}

std::vector<char> burrowsWheelerEncode(const std::vector<char>& input) {
    // BWT
    size_t len = input.size();
    std::vector<const char*> rotations(len);
    for (size_t i = 0; i < len; ++i) {
        rotations[i] = &input[i];
    }

    std::sort(rotations.begin(), rotations.end(), [&input, len](const char* a, const char* b) {
        return std::lexicographical_compare(a, a + len, b, b + len);
    });

    std::vector<char> bwtResult(len);
    for (size_t i = 0; i < len; ++i) {
        bwtResult[i] = *(rotations[i] == &input[0] ? &input.back() : rotations[i] - 1);
    }

    // MTF
    std::list<char> alphabet(input.begin(), input.end());
    alphabet.sort();
    alphabet.unique();

    std::vector<char> mtfResult(len);
    for (size_t i = 0; i < len; ++i) {
        char c = bwtResult[i];
        int position = std::distance(alphabet.begin(), std::find(alphabet.begin(), alphabet.end(), c));
        mtfResult[i] = static_cast<char>(position);
        alphabet.remove(c);
        alphabet.push_front(c);
    }

    return mtfResult;
}

std::vector<char> burrowsWheelerDecode(const std::vector<char>& input) {
    // Inverse MTF
    std::list<char> alphabet(input.begin(), input.end());
    alphabet.sort();
    alphabet.unique();

    size_t len = input.size();
    std::vector<char> invMtfResult(len);
    for (size_t i = 0; i < len; ++i) {
        char pos = input[i];
        auto it = std::next(alphabet.begin(), pos);
        char c = *it;
        invMtfResult[i] = c;
        alphabet.erase(it);
        alphabet.push_front(c);
    }

    // Inverse BWT
    std::vector<std::pair<char, int>> pairs(len);
    for (size_t i = 0; i < len; ++i) {
        pairs[i] = {invMtfResult[i], static_cast<int>(i)};
    }
    std::sort(pairs.begin(), pairs.end());

    std::vector<char> invBwtResult(len);
    int idx = 0;
    for (size_t i = 0; i < len; ++i) {
        invBwtResult[i] = pairs[idx].first;
        idx = pairs[idx].second;
    }

    return invBwtResult;
}


int main() {
    const int numIterations = 10;
    const char* filename = "grand_canyon_alt.geobin";
    const char* runLengthFilename = "run_length_encoded.bin";
    const char* runLengthDecodedFilename = "run_length_decoded.bin";
    const char* lzwEncodedFilename = "lzw_encoded.bin";
    const char* lzwDecodedFilename = "lzw_decoded.bin";
    const char* lzpEncodedFileName = "lzp_encoded.bin";
    const char* lzpDecodedFileName = "lzp_decoded.bin";

    const char* burrowsWheelerFilename = "burrows_wheeler_encoded.bin";
    const char* burrowsWheelerDecodedFilename = "burrows_wheeler_decoded.bin";

    double avgRunLengthSizeBytes = 0;
    double avgEncodedRunLengthTimeMs = 0;
    double avgDecodedRunLengthTimeMs = 0;
    double avgLzwSizeBytes = 0;
    double avgEncodedLzwTimeMs = 0;
    double avgDecodedLzwTimeMs = 0;
    double avgLzpSizeBytes = 0;
    double avgEncodedLzpTimeMs = 0;
    double avgDecodedLzpTimeMs = 0;

    double avgBurrowsWheelerSizeBytes = 0;
    double avgEncodedBurrowsWheelerTimeMs = 0;
    double avgDecodedBurrowsWheelerTimeMs = 0;



    for (int i = 0; i < numIterations; ++i) {

        //generateRandomBinFile(filename, file_size, zeroProbability);

        // Read the binary file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error: Unable to open the file for reading." << std::endl;
            return 1;
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

        // Verify that no data is lost by comparing decoded data with the original data
        bool dataMatches = binaryData == runLengthDecoded;
        std::cout << "RLR Data Matches: " << (dataMatches ? "Yes" : "No") << std::endl;

        // Write the run-length encoded and decoded files
        std::ofstream runLengthOutFile(runLengthFilename, std::ios::binary);
        std::ofstream runLengthDecodedOutFile(runLengthDecodedFilename, std::ios::binary);
        if (!runLengthOutFile || !runLengthDecodedOutFile) {
            std::cerr << "Error: Unable to create the run-length encoded or decoded file." << std::endl;
            return 1;
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
        avgRunLengthSizeBytes += getFileSize(runLengthFilename);
        avgEncodedRunLengthTimeMs += durationEncodRunLength.count();
        avgDecodedRunLengthTimeMs += durationDecodRunLength.count();

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

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzwDataMatches = binaryData == lzwDecoded;
        std::cout << "LZW Data Matches: " << (lzwDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZW encoded data for further verification
        std::ofstream lzwOutFile(lzwEncodedFilename, std::ios::binary);
        std::ofstream lzwDecodedOutFile(lzwDecodedFilename, std::ios::binary);
        if (!lzwOutFile || !lzwDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZW encoded or decoded file." << std::endl;
            return 1;
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
        avgLzwSizeBytes += getFileSize(lzwEncodedFilename);
        avgEncodedLzwTimeMs += durationEncodeLzw.count();
        avgDecodedLzwTimeMs += durationDecodeLzw.count();

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

        // Verify that no data is lost by comparing decoded data with the original data
        bool lzpDataMatches = binaryData == lzpDecoded;
        std::cout << "LZP Data Matches: " << (lzpDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from LZP encoded data for further verification
        std::ofstream lzpOutFile(lzpEncodedFileName, std::ios::binary);
        std::ofstream lzpDecodedOutFile(lzpDecodedFileName, std::ios::binary);
        if (!lzpOutFile || !lzpDecodedOutFile) {
            std::cerr << "Error: Unable to create the LZP encoded or decoded file." << std::endl;
            return 1;
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
        avgLzpSizeBytes += getFileSize(lzpEncodedFileName);
        avgEncodedLzpTimeMs += durationEncodeLzp.count();
        avgDecodedLzpTimeMs += durationDecodeLzp.count();

        // Perform Burrows-Wheeler encoding
        auto startEncodeBurrowsWheeler = std::chrono::high_resolution_clock::now();
        std::vector<char> burrowsWheelerEncoded = burrowsWheelerEncode(binaryData);
        auto stopEncodeBurrowsWheeler = std::chrono::high_resolution_clock::now();
        auto durationEncodeBurrowsWheeler = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodeBurrowsWheeler - startEncodeBurrowsWheeler);

        // Perform Burrows-Wheeler decoding
        auto startDecodeBurrowsWheeler = std::chrono::high_resolution_clock::now();
        std::vector<char> burrowsWheelerDecoded = burrowsWheelerDecode(burrowsWheelerEncoded);
        auto stopDecodeBurrowsWheeler = std::chrono::high_resolution_clock::now();
        auto durationDecodeBurrowsWheeler = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodeBurrowsWheeler - startDecodeBurrowsWheeler);

        // Verify that no data is lost by comparing decoded data with the original data
        bool burrowsWheelerDataMatches = binaryData == burrowsWheelerDecoded;
        std::cout << "Burrows-Wheeler Data Matches: " << (burrowsWheelerDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from Burrows-Wheeler encoded data for further verification
        std::ofstream burrowsWheelerOutFile(burrowsWheelerFilename, std::ios::binary);
        std::ofstream burrowsWheelerDecodedOutFile(burrowsWheelerDecodedFilename, std::ios::binary);
        if (!burrowsWheelerOutFile || !burrowsWheelerDecodedOutFile) {
            std::cerr << "Error: Unable to create the Burrows-Wheeler encoded or decoded file." << std::endl;
            return 1;
        }

        for (size_t i = 0; i < burrowsWheelerEncoded.size(); ++i) {
            char byte = burrowsWheelerEncoded[i];
            burrowsWheelerOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        for (size_t i = 0; i < burrowsWheelerDecoded.size(); ++i) {
            char byte = burrowsWheelerDecoded[i];
            burrowsWheelerDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        burrowsWheelerOutFile.close();
        burrowsWheelerDecodedOutFile.close();

        // open the Burrows-Wheeler encoded file and determine the file size
        avgBurrowsWheelerSizeBytes += getFileSize(burrowsWheelerFilename);
        avgEncodedBurrowsWheelerTimeMs += durationEncodeBurrowsWheeler.count();
        avgDecodedBurrowsWheelerTimeMs += durationDecodeBurrowsWheeler.count();

    }

    avgRunLengthSizeBytes /= numIterations;
    avgEncodedRunLengthTimeMs /= numIterations;
    avgDecodedRunLengthTimeMs /= numIterations;

    avgLzwSizeBytes /= numIterations;
    avgEncodedLzwTimeMs /= numIterations;
    avgDecodedLzwTimeMs /= numIterations;

    avgLzpSizeBytes /= numIterations;
    avgEncodedLzpTimeMs /= numIterations;
    avgDecodedLzpTimeMs /= numIterations;

    avgBurrowsWheelerSizeBytes /= numIterations;
    avgEncodedBurrowsWheelerTimeMs /= numIterations;
    avgDecodedBurrowsWheelerTimeMs /= numIterations;

    std::cout << "Average Run-Length Encoded File Size: " << avgRunLengthSizeBytes << " bytes" << std::endl;
    std::cout << "Average Run-Length Encoding Time: " << avgEncodedRunLengthTimeMs << " ms" << std::endl;
    std::cout << "Average Run-Length Decoding Time: " << avgDecodedRunLengthTimeMs << " ms" << std::endl << std::endl;


    std::cout << "Average LZW Encoded File Size: " << avgLzwSizeBytes << " bytes" << std::endl;
    std::cout << "Average LZW Encoding Time: " << avgEncodedLzwTimeMs << " ms" << std::endl;
    std::cout << "Average LZW Decoding Time: " << avgDecodedLzwTimeMs << " ms" << std::endl << std::endl;

    std::cout << "Average Lempel-Ziv Parallel (LZP) Encoded File Size: " << avgLzpSizeBytes << " bytes" << std::endl;
    std::cout << "Average Lempel-Ziv Parallel (LZP) Encoding Time: " << avgEncodedLzpTimeMs << " ms" << std::endl;
    std::cout << "Average Lempel-Ziv Parallel (LZP) Decoding Time: " << avgDecodedLzpTimeMs << " ms" << std::endl << std::endl;

    std::cout << "Average Burrows-Wheeler Encoded File Size: " << avgBurrowsWheelerSizeBytes << " bytes" << std::endl;
    std::cout << "Average Burrows-Wheeler Encoding Time: " << avgEncodedBurrowsWheelerTimeMs << " ms" << std::endl;
    std::cout << "Average Burrows-Wheeler Decoding Time: " << avgDecodedBurrowsWheelerTimeMs << " ms" << std::endl << std::endl;


    return 0;
}