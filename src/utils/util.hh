#pragma once

#include "spdlog/spdlog.h"
#include <cstdint>
#include <fmt/base.h>
#include <iostream>
#include <zip.h>

// Useful project wide typedefs
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using MemRange = std::pair<u16, u16>;

namespace utility {
inline bool in_range(u16 addr, MemRange range)
{
    return range.first <= addr && addr <= range.second;
}

namespace bitwise {
    inline void set_bit(u8* val, u8 pos)
    {
        *val |= (1 << pos);
    }

    inline void reset_bit(u8* val, u8 pos)
    {
        *val &= ~(1 << pos);
    }

    inline void assign_bit(u8* val, u8 pos, bool on)
    {
        if (on) {
            *val |= (1 << pos);
        } else {
            *val &= ~(1 << pos);
        }
    }

    inline u8 get_bit(u8* val, u8 pos)
    {
        return (*val >> pos) & 0x1;
    }

    inline bool is_bit_set(u8* val, u8 pos)
    {
        return (*val & (1 << pos)) != 0;
    }
};

namespace file_utils {
    inline bool is_gb_extension(std::string& filename)
    {
        int n = filename.size();
        if (n < 3) return false;
        return (filename.substr(n-3) == ".gb");
    }

    inline bool is_zip_extension(std::string& filename)
    {
        int n = filename.size();
        if (n < 4) return false;
        return (filename.substr(n-4) == ".zip");
    }

    inline std::vector<u8> unzip_to_memory(std::string& zip_file)
    {
        // Open the ZIP file for reading
        int err = 0;
        zip_t* zip = zip_open(zip_file.c_str(), 0, &err);
        if (zip == nullptr) {
            std::cerr << "Failed to open ZIP file: " << zip_file << std::endl;
            return {};
        }

        // Get the number of files in the ZIP archive
        zip_int64_t num_files = zip_get_num_entries(zip, 0);
        if (num_files < 0) {
            std::cerr << "Failed to get the number of files in the ZIP archive" << std::endl;
            zip_close(zip);
            return {};
        }

        // Iterate over all files in the ZIP archive
        const char* filename = zip_get_name(zip, 0, 0);
        if (filename == nullptr) {
            std::cerr << "Failed to get filename" << std::endl;
            return {};
        }

        // Print the name of the file being extracted (optional)
        std::cout << "Extracting: " << filename << std::endl;

        // Open the file inside the ZIP archive
        zip_file_t* file = zip_fopen(zip, filename, 0);
        if (file == nullptr) {
            std::cerr << "Failed to open file inside zip: " << filename << std::endl;
            return {};
        }

        // Get the file size
        zip_stat_t file_info;
        if (zip_stat(zip, filename, 0, &file_info) != 0) {
            std::cerr << "Failed to get file info for: " << filename << std::endl;
            zip_fclose(file);
            return {};
        }

        // Allocate a buffer to store the file's contents in memory
        std::vector<u8> buffer(file_info.size);

        // Read the file content into the buffer
        zip_int64_t bytes_read = zip_fread(file, buffer.data(), buffer.size());
        if (bytes_read < 0) {
            std::cerr << "Failed to read file: " << filename << std::endl;
            zip_fclose(file);
            return {};
        }

        // Close the file
        zip_fclose(file);

        // Close the ZIP archive
        zip_close(zip);

        return buffer;
    }
}
};
