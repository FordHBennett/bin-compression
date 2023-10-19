#include "huffman_class.h"

// Constructors
Huffman_Stats::Huffman_Stats() :
    avgSizeBytes(0.0), avgEncodedTimeMs(0.0), avgDecodedTimeMs(0.0),
    avgCompressionRatio(0.0), avgPeakMemoryDuringEncoding(0), avgPeakMemoryDuringDecoding(0),
    avgEncodedThroughput(0.0), avgThroughputDecoded(0.0) {}

Huffman_Stats::~Huffman_Stats() {
    deleteTree(huffmanTreeRoot);
}

std::vector<char> Huffman_Stats::encode(const std::vector<char>& input) {
    std::vector<char> encoded;

    for (const char& ch : input) {
        std::string code = huffmanCodes[ch];
        for (char bit : code) {
            encoded.push_back(bit);
        }
    }

    return encoded;
}

std::vector<char> Huffman_Stats::decode(const std::vector<char>& input) {
    std::vector<char> decoded;
    std::string code;

    for (const char& bit : input) {
        code += bit;
        if (reverseHuffmanCodes.find(code) != reverseHuffmanCodes.end()) {
            decoded.push_back(reverseHuffmanCodes[code]);
            code.clear();
        }
    }

    return decoded;
}

Huffman_Stats::Node* Huffman_Stats::buildHuffmanTree(const std::unordered_map<char, int>& frequencies) {
    auto compare = [](Node* left, Node* right) {
        return left->frequency > right->frequency;
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> pq(compare);

    for (const auto& pair : frequencies) {
        pq.emplace(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        int sum = left->frequency + right->frequency;
        Node* mergedNode = new Node('\0', sum);
        mergedNode->left = left;
        mergedNode->right = right;

        pq.emplace(mergedNode);
    }

    return pq.top();
}

void Huffman_Stats::generateHuffmanCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& codes) {
    if (!root) {
        return;
    }

    if (!root->left && !root->right) {
        codes[root->character] = code;
    }

    generateHuffmanCodes(root->left, code + "0", codes);
    generateHuffmanCodes(root->right, code + "1", codes);
}

void Huffman_Stats::deleteTree(Node* root) {
    if (!root) {
        return;
    }

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// Functions
void Huffman_Stats::printStats() {
    std::cout << "Huffman: Average size in bytes: " << avgSizeBytes << std::endl;
    std::cout << "Huffman: Average encoded time in ms: " << avgEncodedTimeMs << std::endl;
    std::cout << "Huffman: Average decoded time in ms: " << avgDecodedTimeMs << std::endl;
    std::cout << "Huffman: Average compression ratio: " << avgCompressionRatio << std::endl;
    std::cout << "Huffman: Average peak memory during encoding: " << avgPeakMemoryDuringEncoding << std::endl;
    std::cout << "Huffman: Average peak memory during decoding: " << avgPeakMemoryDuringDecoding << std::endl;
    std::cout << "Huffman: Average encoded throughput: " << avgEncodedThroughput << std::endl;
    std::cout << "Huffman: Average throughput decoded: " << avgThroughputDecoded << std::endl;
}

void Huffman_Stats::calculateAvgStats(int divisor){
    avgSizeBytes /= divisor;
    avgEncodedTimeMs /= divisor;
    avgDecodedTimeMs /= divisor;
    avgCompressionRatio /= divisor;
    avgPeakMemoryDuringEncoding /= divisor;
    avgPeakMemoryDuringDecoding /= divisor;
    avgEncodedThroughput /= divisor;
    avgThroughputDecoded /= divisor;
}

void Huffman_Stats::getFileStats(std::vector<char> &binaryData, const char* huffmanEncodedFileName, const char* huffmanDecodedFileName, size_t fileSize){
        auto startEncodehuffman = std::chrono::high_resolution_clock::now();
        std::vector<char> huffmanEncoded = encode(binaryData);
        auto stopEncodehuffman = std::chrono::high_resolution_clock::now();
        auto durationEncodehuffman = std::chrono::duration_cast<std::chrono::milliseconds>(stopEncodehuffman - startEncodehuffman);

        // Perform huffman decoding
        auto startDecodehuffman = std::chrono::high_resolution_clock::now();
        std::vector<char> huffmanDecoded = decode(huffmanEncoded);
        auto stopDecodehuffman = std::chrono::high_resolution_clock::now();
        auto durationDecodehuffman = std::chrono::duration_cast<std::chrono::milliseconds>(stopDecodehuffman - startDecodehuffman);

        // Calculate the peak memory usage
        // avghuffmanStats.setAvgPeakMemoryDuringDecoding(avghuffmanStats.getAvgPeakMemoryDuringDecoding() + getPeakMemoryUsage());
        // avghuffmanStats.setAvgPeakMemoryDuringEncoding(avghuffmanStats.getAvgPeakMemoryDuringEncoding() + getPeakMemoryUsage());

        // Calculate the throughput for encoding and decoding
        avgEncodedThroughput += binaryData.size() / static_cast<double>(durationEncodehuffman.count()) * 1000; // bytes/sec
        avgThroughputDecoded += huffmanDecoded.size() / static_cast<double>(durationDecodehuffman.count()) * 1000; // bytes/sec

        // Verify that no data is lost by comparing decoded data with the original data
        bool huffmanDataMatches = binaryData == huffmanDecoded;
        std::cout << "Huffman Data Matches: " << (huffmanDataMatches ? "Yes" : "No") << std::endl;

        // Create a binary file from huffman encoded data for further verification
        std::ofstream huffmanOutFile(huffmanEncodedFileName, std::ios::binary);
        std::ofstream huffmanDecodedOutFile(huffmanDecodedFileName, std::ios::binary);
        if (!huffmanOutFile || !huffmanDecodedOutFile) {
            std::cerr << "Error: Unable to create the huffman encoded or decoded file." << std::endl;
        }

        for (size_t i = 0; i < huffmanEncoded.size(); ++i) {
            int code = huffmanEncoded[i];
            huffmanOutFile.write(reinterpret_cast<const char*>(&code), sizeof(int));
        }

        for (size_t i = 0; i < huffmanDecoded.size(); ++i) {
            char byte = huffmanDecoded[i];
            huffmanDecodedOutFile.write(reinterpret_cast<const char*>(&byte), sizeof(char));
        }

        huffmanOutFile.close();
        huffmanDecodedOutFile.close();

        // open the huffman encoded file and determine the file size
        avgSizeBytes += getFileSize(huffmanEncodedFileName);
        avgEncodedTimeMs += durationEncodehuffman.count();
        avgDecodedTimeMs += durationDecodehuffman.count();
        avgCompressionRatio += static_cast<double>(getFileSize(huffmanEncodedFileName)) / fileSize;
}

 void Huffman_Stats::getStatsFromEncodingDecodingFunctions(const char* filename, int numIterations) {
    std::cout << "Compressing " << filename << " using huffman" << std::endl;
    const char* huffmanEncodedFilename = "huffman_encoded.bin";
    const char* huffmanDecodedFilename = "huffman_decoded.bin";

     Huffman_Stats avghuffmanStats;

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

        // Perform huffman encoding and decoding
        avghuffmanStats.getFileStats(binaryData, huffmanEncodedFilename, huffmanDecodedFilename, fileSize);
    }

    // Calculate the average stats for the current file
    avghuffmanStats.calculateAvgStats(numIterations);

    avgSizeBytes += avghuffmanStats.getAvgSizeBytes();
    avgEncodedTimeMs += avghuffmanStats.getAvgEncodedTimeMs();
    avgDecodedTimeMs += avghuffmanStats.getAvgDecodedTimeMs();
    avgCompressionRatio += avghuffmanStats.getAvgCompressionRatio();
    avgPeakMemoryDuringEncoding += avghuffmanStats.getAvgPeakMemoryDuringEncoding();
    avgPeakMemoryDuringDecoding += avghuffmanStats.getAvgPeakMemoryDuringDecoding();
    avgEncodedThroughput += avghuffmanStats.getAvgEncodedThroughput();
    avgThroughputDecoded += avghuffmanStats.getAvgThroughputDecoded();
 }

// Setters
void Huffman_Stats::setAvgSizeBytes(double value) { avgSizeBytes = value; }
void Huffman_Stats::setAvgEncodedTimeMs(double value) { avgEncodedTimeMs = value; }
void Huffman_Stats::setAvgDecodedTimeMs(double value) { avgDecodedTimeMs = value; }
void Huffman_Stats::setAvgCompressionRatio(double value) { avgCompressionRatio = value; }
void Huffman_Stats::setAvgPeakMemoryDuringEncoding(size_t value) { avgPeakMemoryDuringEncoding = value; }
void Huffman_Stats::setAvgPeakMemoryDuringDecoding(size_t value) { avgPeakMemoryDuringDecoding = value; }
void Huffman_Stats::setAvgEncodedThroughput(double value) { avgEncodedThroughput = value; }
void Huffman_Stats::setAvgThroughputDecoded(double value) { avgThroughputDecoded = value; }

// Getters
double Huffman_Stats::getAvgSizeBytes() const { return avgSizeBytes; }
double Huffman_Stats::getAvgEncodedTimeMs() const { return avgEncodedTimeMs; }
double Huffman_Stats::getAvgDecodedTimeMs() const { return avgDecodedTimeMs; }
double Huffman_Stats::getAvgCompressionRatio() const { return avgCompressionRatio; }
size_t Huffman_Stats::getAvgPeakMemoryDuringEncoding() const { return avgPeakMemoryDuringEncoding; }
size_t Huffman_Stats::getAvgPeakMemoryDuringDecoding() const { return avgPeakMemoryDuringDecoding; }
double Huffman_Stats::getAvgEncodedThroughput() const { return avgEncodedThroughput; }
double Huffman_Stats::getAvgThroughputDecoded() const { return avgThroughputDecoded; }