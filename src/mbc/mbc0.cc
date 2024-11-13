// mbc0.cc
//
// A cartridge with no memory bank controller

#include "mbc/mbc0.hh"
#include "cartridge.hh"
#include "mbc/mbc.hh"

namespace gameboymebob {
class Cartridge;

Mbc0::Mbc0(Cartridge* c)
    : MbcInterface(c)
{
}

u8 Mbc0::read_byte(u16 addr)
{
    // Just pass the byte through directly
    return cart->rom[addr];
}

void Mbc0::write_byte(u16 addr, u8 byte)
{
    // Ignore this write, we are truly ROM
    return;
}

};
