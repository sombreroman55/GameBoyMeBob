#include "mbc/mbc1.hh"
#include "cartridge.hh"
#include "mbc/mbc.hh"
#include "utils/util.hh"

namespace gameboymebob {

u8 Mbc1::read_byte(u16 addr)
{
    u16 effective_addr = addr;
    if (utility::in_range(addr, mbc1_read_map::rom_bank_0)) {
        if (mode) {
            effective_addr = 0x4000 * zero_bank + addr;
        }
    }
    else if (utility::in_range(addr, mbc1_read_map::rom_bank_N)) {
        effective_addr = 0x4000 * high_bank + (0x4000 - addr);
    }
    else if (utility::in_range(addr, mbc1_read_map::ram)) {
        if (_ram_enabled) {
            effective_addr = addr - 0xA000;
        } else {
            return 0xFF; // return junk
        }
    }
    return cart->rom_data[effective_addr];
}

void Mbc1::write_byte(u16 addr, u8 byte)
{
    if (utility::in_range(addr, mbc1_write_map::enable_ram)) {
        _ram_enabled = (byte & 0x0F) == 0x0A;
    }
    else if (utility::in_range(addr, mbc1_write_map::rom_bank_select)) {
    }
    else if (utility::in_range(addr, mbc1_write_map::ram_bank_select)) {
        ram_bank = (byte & 0x03);
    }
    else if (utility::in_range(addr, mbc1_write_map::mode_select)) {
        mode = (byte & 0x01);
    }
    else if (utility::in_range(addr, mbc1_write_map::ram)) {
        if (_ram_enabled) {
            // TODO: Write byte
        }
    }
}

};
