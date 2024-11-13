#include "mbc/mbc1.hh"
#include "cartridge.hh"
#include "mbc/mbc.hh"
#include "utils/util.hh"

namespace gameboymebob {

Mbc1::Mbc1(Cartridge* c)
    : MbcInterface(c)
{
}

u8 Mbc1::read_byte(u16 addr)
{
    u16 effective_addr = addr;
    if (utility::in_range(addr, mbc1_read_map::rom_bank_0)) {
        if (mode) {
            effective_addr = 0x4000 * zero_bank + addr;
        }
    } else if (utility::in_range(addr, mbc1_read_map::rom_bank_N)) {
        effective_addr = 0x4000 * high_bank + (0x4000 - addr);
    } else if (utility::in_range(addr, mbc1_read_map::ram)) {
        if (_ram_enabled) {
            if (cart->header.ram_size == 2048 || cart->header.ram_size == 8192) {
                effective_addr = (addr - 0xA000) % cart->header.ram_size;
            } else if (cart->header.ram_size == 32768) {
                if (mode) {
                    effective_addr = 0x2000 * ram_bank + (addr - 0xA000);
                } else {
                    effective_addr = addr - 0xA000;
                }
            }
        } else {
            return 0xFF; // return junk
        }
    }
    return cart->rom[effective_addr];
}

void Mbc1::write_byte(u16 addr, u8 byte)
{
    if (utility::in_range(addr, mbc1_write_map::enable_ram)) {
        _ram_enabled = (byte & 0x0F) == 0x0A;
    } else if (utility::in_range(addr, mbc1_write_map::rom_bank_select)) {
    } else if (utility::in_range(addr, mbc1_write_map::ram_bank_select)) {
        ram_bank = (byte & 0x03);
    } else if (utility::in_range(addr, mbc1_write_map::mode_select)) {
        mode = (byte & 0x01);
    } else if (utility::in_range(addr, mbc1_write_map::ram)) {
        if (_ram_enabled) {
            u16 effective_addr = addr;
            if (cart->header.ram_size == 2048 || cart->header.ram_size == 8192) {
                effective_addr = (addr - 0xA000) % cart->header.ram_size;
            } else if (cart->header.ram_size == 32768) {
                if (mode) {
                    effective_addr = 0x2000 * ram_bank + (addr - 0xA000);
                } else {
                    effective_addr = addr - 0xA000;
                }
            }
            cart->ram[effective_addr] = byte;
        }
    }
}

};
