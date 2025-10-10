#include "mbc/mbc1.hh"
#include "cartridge.hh"
#include "mbc/mbc.hh"
#include "utils/util.hh"

namespace gameboymebob {

Mbc1::Mbc1(Cartridge* c)
    : MbcInterface(c)
{
}

u8 Mbc1::get_adjusted_rom_bank(u8 bank)
{
    if (cart->header.rom_size >= 0x100000) {
        bank |= (ram_bank << 5);
    }

    uint8_t numBits = static_cast<uint8_t>(std::log2(cart->header.rom_size / 0x4000));
    uint8_t bitMask = (1 << numBits) - 1;
    return bank &= bitMask;
}

u8 Mbc1::read_byte(u16 addr)
{
    if (utility::in_range(addr, mbc1_read_map::rom_bank_0)) {
        u8 bank = mode ? get_adjusted_rom_bank(0) : 0;
        return cart->rom[0x4000 * bank + addr];
    } else if (utility::in_range(addr, mbc1_read_map::rom_bank_N)) {
        return cart->rom[0x4000 * get_adjusted_rom_bank(rom_bank) + (addr - 0x4000)];
    } else if (utility::in_range(addr, mbc1_read_map::ram)) {
        if (ram_enabled) {
            u8 bank = mode ? ram_bank : 0;
            return cart->ram[0x2000 * bank + (addr - 0xA000)];
        }
    }
    return 0xFF; // open bus
}

void Mbc1::write_byte(u16 addr, u8 byte)
{
    if (utility::in_range(addr, mbc1_write_map::enable_ram)) {
        // only enable RAM if there is actually RAM to enable
        ram_enabled = cart->header.ram_size > 0 && ((byte & 0x0F) == 0x0A);
    } else if (utility::in_range(addr, mbc1_write_map::rom_bank_select)) {
        rom_bank = std::max((byte & 0x1F), 1);
    } else if (utility::in_range(addr, mbc1_write_map::ram_bank_select)) {
        ram_bank = (byte & 0x03);
    } else if (utility::in_range(addr, mbc1_write_map::mode_select)) {
        mode = (byte & 0x01);
    } else if (utility::in_range(addr, mbc1_write_map::ram)) {
        if (cart->header.ram_size > 0 && ram_enabled) {
            u8 bank = mode ? ram_bank : 0;
            cart->ram[0x2000 * bank + (addr - 0xA000)] = byte;
        }
    }
}

};
