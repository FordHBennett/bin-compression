#pragma once
#include <cstddef>
#include <vector>
#include <functional>
#include <unordered_map>
#include <array>
#include <filesystem>

enum class Side {
    s0,
    s1,
    s2,
    s3,
    s4,
    s5,
    NUMBER_SIDES
};

class CommonStats {
    private:
        //member variables
        size_t average_file_size = 0.0;
        size_t average_time_encoded_in_s  = 0.0;
        size_t average_time_decoded_in_s  = 0.0;
        size_t average_compression_ratio = 0.0;
        size_t average_encoded_throughput = 0.0;
        size_t average_decoded_throughput = 0.0;
        int8_t data_type_byte_size = 0;

        std::array<std::array<uint16_t,4>, static_cast<size_t>(Side::NUMBER_SIDES)> side_resolutions;


        //create a look up table full of data_type and their corresponding size

    public:
        CommonStats();
        CommonStats(const CommonStats& other);
        CommonStats& operator=(const CommonStats& other);
        ~CommonStats();
        //member functions
        void Calculate_Cumulative_Average_Stats_For_Directory(int divisors, int files);
        void Compute_Time_Encoded(std::function<void(std::vector<char>)> encode, std::vector<char> data);
            // when computing the encoded time added the calculated time to average_time_encoded_in_s
            // Do the same for the reset of the compute functions

        void Compute_Time_Decoded(std::function<void(std::vector<char>)> decode, std::vector<char> data);
        void Compute_Compression_Ratio(const std::filesystem::path original_file_path, const std::filesystem::path compressed_file_path);
        void Compute_File_Size(const std::filesystem::path file_path);

        void Compute_Encoded_Throughput(){
            average_encoded_throughput = average_file_size / average_time_encoded_in_s ;
        }

        void Compute_Decoded_Throughput(){
            average_decoded_throughput = average_file_size / average_time_decoded_in_s ;
        }
        void Set_Data_Type_Size_And_Side_Resolutions(const std::filesystem::path geometa_path);
        const void Print_Stats(const char* compressionType) const;

        const bool Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& originalData, const std::vector<char>& decodedData) const{
            return originalData == decodedData;
        }

        //getters
        int Get_Side_Resolution(const char* side, const uint8_t c_number) const{
            return side_resolutions[static_cast<size_t>(static_cast<Side>(std::stoi(side)))][c_number];
        }

        int Get_Data_Type_Size() const {return data_type_byte_size;}


};
