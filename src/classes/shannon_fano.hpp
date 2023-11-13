#pragma once


#include "common_stats.hpp"
#include <vector>

class ShannonFano : public CommonStats {
    public:
        ShannonFano();
        void Write_Frequencies_To_JSON_File(const std::filesystem::path& binary_file_path, const std::filesystem::path& json_file_path) const;

    private:
        const char* compression_type = "ShannonFano";
};