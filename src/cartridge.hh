#pragma once

#include "utils/util.hh"
#include "mbc/mbc.hh"
#include <string>
#include <unordered_map>

// TODO: Rework the header to get populated with the "meanings" of all the numbers,
//       and leave the actual binary values as simply part of the rom_data in the cart
namespace gameboymebob {
// clang-format off
class CartridgeHeader {
friend class Cartridge;
private:
    const std::array<std::string, 2> destinations = {
        "Japan",
        "Overseas only"
    };

    // TODO: Populate the licensee strings
    const std::unordered_map<u8, std::string> old_licensees = {
        { 0x00, "" },
    };

    // TODO: Populate the licensee strings
    const std::unordered_map<std::string, std::string> new_licensees = {
        { 0x00, "" },
    };

    u8 title[16];               // 0134 - 0143 (is only 12 bytes on newer games)
    char manufacturer[4];       // 013F - 0142 (overlaps title region on newer games)
    u8 cgb_flag;                // 0143 (also overlaps title region on newer games)
    u8 new_licensee_code[2];    // 0144-0145 (ASCII encoded numbers!!)
    u8 sgb_flag;                // 0146
    u8 cart_type;               // 0147
    u8 rom_size;                // 0148
    u8 ram_size;                // 0149
    u8 destination_code;        // 014A
    u8 old_licensee_code;       // 014B (Actual hex value!!)
    u8 version_number;          // 014C
    u8 checksum;                // 014D
    u16 global_checksum;        // 014E-14F (big endian!)

public:
    std::string get_title(void)
    {
    }

    std::string get_manufacturer(void)
    {
    }

    bool supports_cgb(void)
    {
    }

    std::string get_licensee_code(void)
    {
        if (old_licensee_code == 0x33) {
            std::string new_licensee_key((char*)new_licensee_code);
            return new_licensees.at(new_licensee_key);
        }
        return old_licensees.at(old_licensee_code);
    }

    std::string get_destination(void)
    {
        return destinations[destination_code];
    }

    bool supports_sgb(void)
    {
        return sgb_flag != 0x03;
    }

    void print(void)
    {
    }

    u32 get_rom_size(void)
    {
    }

    u32 get_ram_size(void)
    {
    }

};
// clang-format on

class Cartridge {
private:
    MbcInterface* bank_controller = nullptr;

public:
    std::vector<u8> rom_data;
    CartridgeHeader header;

    void parse_header(void);
    Cartridge(std::string rom);
    ~Cartridge();

    u8 read_byte(u16 addr);
    void write_byte(u16 addr, u8 byte);
};
};
