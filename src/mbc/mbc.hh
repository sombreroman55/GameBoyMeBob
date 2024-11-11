#pragma once

#include "utils/util.hh"

namespace gameboymebob {

// forward declaration for compiler
class Cartridge;

class MbcInterface {
protected:
    Cartridge* cart = nullptr;

public:
    MbcInterface(Cartridge* c)
        : cart(c)
    {
    }
    virtual u8 read_byte(u16 addr) = 0;
    virtual void write_byte(u16 addr, u8 byte) = 0;
};
};
