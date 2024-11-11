#pragma once

#include "mbc/mbc.hh"

namespace gameboymebob {

// clang-format off
namespace mbc1_write_map {
    constexpr MemRange enable_ram      = { 0x0000, 0x1FFF };
    constexpr MemRange rom_bank_select = { 0x2000, 0x3FFF };
    constexpr MemRange ram_bank_select = { 0x4000, 0x5FFF };
    constexpr MemRange mode_select     = { 0x6000, 0x7FFF };
    constexpr MemRange ram             = { 0xA000, 0xBFFF };
};

namespace mbc1_read_map {
    constexpr MemRange rom_bank_0 = { 0x0000, 0x3FFF };
    constexpr MemRange rom_bank_N = { 0x4000, 0x7FFF };
    constexpr MemRange ram        = { 0xA000, 0xBFFF };
};
// clang-format on

class Mbc1 : public MbcInterface {
private:
    u8 zero_bank = 0;
    u8 high_bank = 0;
    u8 rom_bank = 0;
    u8 ram_bank = 0;
    u8 mode = 0;
    bool _ram_enabled = false;
    u32 rom_size = 0;
    u32 ram_size = 0;

public:
    Mbc1(Cartridge* c);
    u8 read_byte(u16 addr);
    void write_byte(u16 addr, u8 byte);
};
};
