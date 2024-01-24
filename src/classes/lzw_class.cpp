#include "common_stats.hpp"
#include "../functions/file_functions.hpp"
#include "lzw_class.hpp"
#include <lzwfile.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cassert>

//THIS CURRENTLY DOES NOT READ IN A FILE
//SO FIX THAT

#define ERROR_MSG(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << '\n'; \

#define ERROR_MSG_AND_EXIT(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << '\n'; \
    std::exit(EXIT_FAILURE);

#define CHUNK_SIZE (16*1024)

// Constructors
LZW::LZW() {}

#include "LZW.h"
#include <unordered_map>
#include <string>

void LZW::Encoding_Using_LZW() {
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, i)] = i;
    }

    std::string sequence;
    char byte;
    int dict_size = 256;

    for (char byte : binary_data_vec) {
        std::string new_sequence = sequence + byte;
        if (dictionary.count(new_sequence)) {
            sequence = new_sequence;
        } else {
            encoded_data_vec.push_back(dictionary[sequence]);
            dictionary[new_sequence] = dict_size++;
            sequence = std::string(1, byte);
        }
    }

    if (!sequence.empty()) {
        encoded_data_vec.push_back(dictionary[sequence]);
    }
}


void LZW::Decoding_Using_LZW() {
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, i);
    }

    int dict_size = 256;
    std::string sequence(1, encoded_data_vec.front());
    decoded_data_vec.push_back(sequence[0]);
    // int oldCode = encoded_data_vec.front();
    int new_code;

    for (size_t i = 1; i < encoded_data_vec.size(); ++i) {
        new_code = encoded_data_vec[i];
        std::string entry;
        if (dictionary.count(new_code)) {
            entry = dictionary[new_code];
        } else if (new_code == dict_size) {
            entry = sequence + sequence[0];
        } else {
            throw std::runtime_error("Bad compressed code");
        }

        decoded_data_vec.insert(decoded_data_vec.end(), entry.begin(), entry.end());

        dictionary[dict_size++] = sequence + entry[0];
        sequence = entry;
    }
}



const char* lzw::Get_Compression_Type() const {
    return this->compression_type;
}

std::vector<char> lzw::Get_Encoded_Data() const {
    return this->encoded_data;
}

std::vector<char> lzw::Get_Decoded_Data() const {
    return this->decoded_data;
}