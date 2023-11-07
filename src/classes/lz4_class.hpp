#pragma once
#include "common_stats.hpp"
// #include "../functions/file_functions.hpp"
// #include "lz4.h"

class LZ4 : public CommonStats {
    public:
        // Constructors
        LZ4();

        //compress function (size_t max_bytes, size_t ringBufferBytes)
        //create a lz4 stream
        // void encode(std::ifstream& input_stream, const size_t& max_bytes, const size_t& ring_buffer_bytes);

        // //decompress function
        // void decode(const size_t& max_bytes, const size_t& ring_buffer_bytes);

        // void Write_Compressed_File(const std::vector<char>& encoded_data, const char* filename);

        // void Write_Decompressed_File(const std::vector<char>& decoded_data, const char* filename);

        const int Compress_File(FILE* input_file_ptr, FILE* output_file_ptr);

        const int Decompress_File(FILE* input_file_ptr, FILE* output_file_ptr);



        const char* Get_Compression_Type() const;

        std::vector<char> Get_Encoded_Data() const;

        std::vector<char> Get_Decoded_Data() const;

    private:
        std::vector<char> encoded_data;
        std::vector<char> decoded_data;
        const char* compression_type = "lz4";
};
