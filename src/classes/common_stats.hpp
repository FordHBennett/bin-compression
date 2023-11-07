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
        double average_file_size = 0.0;
        size_t average_time_encoded_in_ns = 0;
        size_t average_time_decoded_in_ns = 0;
        size_t average_compression_ratio = 0.0;
        size_t average_encoded_throughput = 0.0;
        size_t average_decoded_throughput = 0.0;
        int8_t data_type_byte_size = 0;

        std::array<std::array<uint16_t,4>, static_cast<size_t>(Side::NUMBER_SIDES)> side_resolutions;


        //create a look up table full of data_type and their corresponding size

    public:
        //default constructor
        CommonStats();
        //copy constructor
        CommonStats(const CommonStats& other);
        //copy assignment operator
        CommonStats& operator=(const CommonStats& other);
        //move constructor
        CommonStats(CommonStats&& other);

        //member functions
        void Calculate_Cumulative_Average_Stats_For_Directory(const int& divisors, const int& files);
        // void Compute_Time_Encoded(const std::function<void(std::vector<char>)>& encode, const std::vector<char>& data);
        // void Compute_Time_Decoded(const std::function<void(std::vector<char>)>& decode, const std::vector<char>& data);
        template <typename EncodeFunction>
        void Compute_Time_Encoded(EncodeFunction encode, const std::vector<char>& data);
        template <typename DecodeFunction>
        void Compute_Time_Decoded(DecodeFunction decode, const std::vector<char>& data);
        void Compute_Compression_Ratio(const std::filesystem::path& original_file_path, const std::filesystem::path& compressed_file_path);
        void Compute_File_Size(const std::filesystem::path& file_path);
        void Compute_Encoded_Throughput();
        void Compute_Decoded_Throughput();
        void Set_Data_Type_Size_And_Side_Resolutions(const std::filesystem::path& geometa_path);
        const void Print_Stats(const char* compressionType) const;
        const bool Is_Decoded_Data_Equal_To_Original_Data(const std::vector<char>& original_vec, const std::vector<char>& decoded_vec) const;

        //getters
        const int64_t Get_Side_Resolution(const uint8_t& lod_number) const;
        const int Get_Data_Type_Size() const;
};
