#include "common_stats.hpp"
#include "../functions/file_functions.hpp"
#include "lz4_class.hpp"
#include <lz4file.h>

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
LZ4::LZ4() {}

// //this probably not right
// void LZ4::encode(std::ifstream& input_stream, const size_t& max_bytes, const size_t& ring_buffer_bytes) {
//     LZ4_stream_t* const lz4_stream = LZ4_createStream();
//     const size_t compare_buffer_bytes = LZ4_COMPRESSBOUND(max_bytes);
//     // char* const compare_buffer_ptr = (char*) malloc(compare_buffer_bytes);
//     // char* const input_buffer_ptr = (char*) malloc(ring_buffer_bytes);
//     std::unique_ptr<char[]> compare_buffer_ptr(new char[compare_buffer_bytes]);
//     std::unique_ptr<char[]> input_buffer_ptr(new char[ring_buffer_bytes]);
//     int input_offset = 0;

//     for( ; ; ){
//         // int input_bytes = (int) strlen(inpPtr);
//         std::string input_line;
//         if (!std::getline(input_stream, input_line)) {
//             // Handle the error or break the loop if end-of-file is reached.
//             break;
//         }
//         int input_bytes = static_cast<int>(std::strlen(input_buffer_ptr.get()));
//         const int compare_bytes = LZ4_compress_fast_continue(lz4_stream, input_buffer_ptr.get(), compare_buffer_ptr.get(),
//                                                             static_cast<int>(input_bytes), compare_buffer_bytes, 1);
//         //add compare_bytes to encoded_data
//         encoded_data.emplace_back(static_cast<char>(compare_bytes));

//         for(int i = 0; i < compare_bytes; i++){
//             encoded_data.emplace_back(compare_buffer_ptr[i]);
//         }

//         input_offset += input_bytes;

//         if(input_offset >= (ring_buffer_bytes - max_bytes)){
//             input_offset = 0;
//         }
//     }

//     encoded_data.emplace_back(static_cast<char>(0));
//     LZ4_freeStream(lz4_stream);
// }

// void LZ4::decode(const size_t& max_bytes, const size_t& ring_buffer_bytes){
//     LZ4_streamDecode_t* const lz4_decode_stream = LZ4_createStreamDecode();
//     // char* const cmpBuf = (char*) malloc(LZ4_COMPRESSBOUND(messageMaxBytes));
//     // char* const decBuf = (char*) malloc(ringBufferBytes);
//     std::unique_ptr<char[]> compare_buffer(new char[LZ4_COMPRESSBOUND(max_bytes)]);
//     std::unique_ptr<char[]> decompressed_buffer(new char[ring_buffer_bytes]);
//     int decompressed_offset = 0;

//     for( ; ; ){
//         int compare_bytes = 0;

//         char* decompressed_buffer_ptr = decompressed_buffer.get();
//         const int decompressed_bytes = LZ4_decompress_safe_continue(lz4_decode_stream, compare_buffer.get(),
//                                                                     decompressed_buffer_ptr, compare_bytes, max_bytes);
//         if(decompressed_bytes < 0){
//             std::cerr << "Error: LZ4 decompression failed." << '\n';
//             ERROR_MSG_AND_EXIT("LZ4 decompression failed.")
//         }

//         for(int i = 0; i < decompressed_bytes; i++){
//             decoded_data.emplace_back(decompressed_buffer_ptr[i]);
//         }

//         decompressed_offset += decompressed_bytes;

//         if(decompressed_offset >= (ring_buffer_bytes - max_bytes)){
//             decompressed_offset = 0;
//         }
//     }

//     LZ4_freeStreamDecode(lz4_decode_stream);
// }

// void LZ4::Write_Compressed_File(const std::vector<char>& encoded_data, const char* filename) {
//     std::ofstream encodedOutFile(filename, std::ios::binary);
//     if (!encodedOutFile) {
//         ERROR_MSG_AND_EXIT("Unable to create the LZ4 encoded file.");
//     }

//     // Writing encoded data to file
//     encodedOutFile.write(encoded_data.data(), encoded_data.size());

//     encodedOutFile.close();
// }

// void LZ4::Write_Decompressed_File(const std::vector<char>& decoded_data, const char* filename) {
//     std::ofstream decodedOutFile(filename, std::ios::binary);
//     if (!decodedOutFile) {
//         ERROR_MSG_AND_EXIT("Unable to create the LZ4 decoded file.");
//     }

//     // Writing decoded data to file
//     decodedOutFile.write(decoded_data.data(), decoded_data.size());

//     decodedOutFile.close();
// }


const int LZ4::Compress_File(FILE* input_file_ptr, FILE* output_file_ptr){
    assert(input_file_ptr != NULL);
    assert(output_file_ptr != NULL);
    LZ4F_errorCode_t ret = LZ4F_OK_NoError;
    size_t len;
    LZ4_writeFile_t* lz4f_write;
    // void* const buf = malloc(CHUNK_SIZE);
    //rewrite the above line to use unique_ptr
    auto buffer = std::make_unique<char[]>(CHUNK_SIZE);
    // check if memory allocation failed
    if(!buffer){
        ERROR_MSG_AND_EXIT("Memory allocation failed.")
    }

    ret = LZ4F_writeOpen(&lz4f_write, output_file_ptr, NULL);

    if(LZ4F_isError(ret)){
        ERROR_MSG_AND_EXIT("LZ4F_writeOpen failed.")
    }

    while(true){
        len = fread(buffer.get(), 1, CHUNK_SIZE, input_file_ptr);

        if(ferror(input_file_ptr)){
            ERROR_MSG("fread failed.")
            goto out;
        }

        /* nothing to read */
        if (len == 0) {
            break;
        }

        ret = LZ4F_write(lz4f_write, buffer.get(), len);
        if(LZ4F_isError(ret)){
            ERROR_MSG(std::string{"LZ4F_write failed."} + std::string{LZ4F_getErrorName(ret)});
            goto out;
        }

    }

out:
    if (LZ4F_isError(LZ4F_writeClose(lz4f_write))){
        ERROR_MSG_AND_EXIT(std::string{"LZ4F_writeClose failed."} + std::string{LZ4F_getErrorName(ret)})
    }

    return 0;
}

const int LZ4::Decompress_File(FILE* input_file_ptr, FILE* output_file_ptr){
    assert(input_file_ptr != NULL);
    assert(output_file_ptr != NULL);
    LZ4F_errorCode_t ret = LZ4F_OK_NoError;
    size_t len;
    LZ4_readFile_t* lz4f_read;
    // void* const buf = malloc(CHUNK_SIZE);
    //rewrite the above line to use unique_ptr
    auto buffer = std::make_unique<char[]>(CHUNK_SIZE);
    // check if memory allocation failed
    if(!buffer){
        ERROR_MSG_AND_EXIT("Memory allocation failed.")
    }

    ret = LZ4F_readOpen(&lz4f_read, output_file_ptr);
    if(LZ4F_isError(ret)){
        ERROR_MSG_AND_EXIT(std::string{"LZ4F_readOpen failed."} + std::string{LZ4F_getErrorName(ret)})
    }

    while(true){
        ret = LZ4F_read(lz4f_read, buffer.get(), CHUNK_SIZE);
        if(LZ4F_isError(ret)){
            ERROR_MSG(std::string{"LZ4F_read failed."} + std::string{LZ4F_getErrorName(ret)})
            goto out;
        }

        if(ret == 0){
            break;
        }

        if(fwrite(buffer.get(), 1, ret, output_file_ptr) != ret){
            ERROR_MSG("fwrite failed.")
            goto out;
        }


    }

out:
    if (LZ4F_isError(LZ4F_readClose(lz4f_read))){
        ERROR_MSG_AND_EXIT(std::string{"LZ4F_readClose failed."} + std::string{LZ4F_getErrorName(ret)})
    }

    if (ret) {
        return 1;
    }

    return 0;
}



const char* LZ4::Get_Compression_Type() const {
    return this->compression_type;
}

std::vector<char> LZ4::Get_Encoded_Data() const {
    return this->encoded_data;
}

std::vector<char> LZ4::Get_Decoded_Data() const {
    return this->decoded_data;
}