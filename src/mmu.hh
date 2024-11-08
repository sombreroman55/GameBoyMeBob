#pragma once

#include "cartridge.hh"
#include "utils/util.hh"
#include <array>

namespace gameboymebob {

// forward declarations for compiler
class SerialController;


class Mmu {
private:
    void write_io_byte(u16 addr, u8 byte);

public:
    std::array<u8, 0x10000> memory = {};

    // Memory mapped components
    SerialController* serial = nullptr;

    Mmu();
    ~Mmu();

    // Device mapping functions
    void map_serial(SerialController* ser);

    u8 read_byte(u16 addr);
    u16 read_word(u16 addr);

    void write_byte(u16 addr, u8 byte);
    void write_word(u16 addr, u16 word);

    void load_cartridge(Cartridge* cart);
};
};
