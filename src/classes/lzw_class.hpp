#pragma once
#include "common_stats.hpp"
// #include "../functions/file_functions.hpp"
// #include "lzw.h"

class LZW : public CommonStats {
    public:
        // Constructors
        LZW();

        //compress function (size_t max_bytes, size_t ringBufferBytes)
        //create a lzw stream
        // void encode(std::ifstream& input_stream, const size_t& max_bytes, const size_t& ring_buffer_bytes);

        // //decompress function
        // void decode(const size_t& max_bytes, const size_t& ring_buffer_bytes);

        // void Write_Compressed_File(const std::vector<char>& encoded_data, const char* filename);

        // void Write_Decompressed_File(const std::vector<char>& decoded_data, const char* filename);

        void Encoding_Using_LZW();

        void Decoding_Using_LZW();



        const char* Get_Compression_Type() const;

        std::vector<char> Get_Encoded_Data() const;

        std::vector<char> Get_Decoded_Data() const;

    private:
        std::vector<char> binary_data_vec = {0};
        std::vector<char> encoded_data_vec = {0};
        std::vector<char> decoded_data_vec = {0};
        const char* compression_type = "lzw";
};
