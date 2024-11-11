// mbc0.cc
//
// A cartridge with no memory bank controller

#include "mbc/mbc0.hh"
#include "cartridge.hh"

namespace gameboymebob {
class Cartridge;

u8 Mbc0::read_byte(u16 addr)
{
    // Just pass the byte through directly
    return cart->rom_data[addr];
}

void Mbc0::write_byte(u16 addr, u8 byte)
{
    // Ignore this write, we are truly ROM
    return;
}

};
