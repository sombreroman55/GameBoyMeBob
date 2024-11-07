#pragma once

#include "cartridge.hh"
#include "utils/util.hh"
#include <array>

namespace gameboymebob {
class Mmu {
public:
    std::array<u8, 0x10000> memory;

    Mmu();
    ~Mmu();

    u8 read_byte(u16 addr);
    u16 read_word(u16 addr);

    void write_byte(u16 addr, u8 byte);
    void write_word(u16 addr, u16 word);

    void load_cartridge(Cartridge* cart);
};
};
