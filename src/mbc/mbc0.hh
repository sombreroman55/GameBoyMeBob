#pragma once

#include "mbc/mbc.hh"

namespace gameboymebob {
class Mbc0 : public MbcInterface {
public:
    Mbc0(Cartridge* c);
    u8 read_byte(u16 addr);
    void write_byte(u16 addr, u8 byte);
};
};
