#include "mbc/mbc1.hh"
#include "cartridge.hh"
#include "mbc/mbc.hh"
#include "utils/util.hh"

namespace gameboymebob {

Mbc1::Mbc1(Cartridge* c)
    : MbcInterface(c)
{
    // spdlog::set_level(spdlog::level::debug);
}

u8 Mbc1::read_byte(u16 addr)
{
    u32 effective_addr = addr;
    if (utility::in_range(addr, mbc1_read_map::rom_bank_0)) {
        if (mode) {
            effective_addr = 0x4000 * (ram_bank << 5) + addr;
        }
    } else if (utility::in_range(addr, mbc1_read_map::rom_bank_N)) {
        effective_addr = 0x4000 * high_bank + (addr - 0x4000);
    } else if (utility::in_range(addr, mbc1_read_map::ram)) {
        spdlog::debug("Reading address {:X}", addr);
        if (ram_enabled) {
            if (cart->header.ram_size == 0x800 || cart->header.ram_size == 0x2000) {
                effective_addr = (addr - 0xA000) % cart->header.ram_size;
            } else if (cart->header.ram_size == 0x8000) {
                if (mode) {
                    effective_addr = 0x2000 * ram_bank + (addr - 0xA000);
                } else {
                    effective_addr = addr - 0xA000;
                }
            }
            return cart->ram[effective_addr];
        } else {
            spdlog::debug("Reading external RAM? Why?");
            return 0xFF; // return junk
        }
    }
    return cart->rom[effective_addr];
}

void Mbc1::write_byte(u16 addr, u8 byte)
{
    if (utility::in_range(addr, mbc1_write_map::enable_ram)) {
        spdlog::info("Writing {:02X} to ram_enable", byte);
        ram_enabled = (cart->header.ram_size > 0 && (byte & 0x0F) == 0x0A);
    } else if (utility::in_range(addr, mbc1_write_map::rom_bank_select)) {
        switch (cart->header.rom_size) {
        case 0x8000:
            rom_bank = (byte & 0x01);
            high_bank = rom_bank;
            break;
        case 0x10000:
            rom_bank = (byte & 0x03);
            high_bank = rom_bank;
            break;
        case 0x20000:
            rom_bank = (byte & 0x07);
            high_bank = rom_bank;
            break;
        case 0x40000:
            rom_bank = (byte & 0x0F);
            high_bank = rom_bank;
            break;
        case 0x80000:
            rom_bank = (byte & 0x1F);
            high_bank = rom_bank;
            break;
        case 0x100000:
            rom_bank = (byte & 0x1F);
            high_bank = ((ram_bank & 0x01) << 5) | rom_bank;
            break;
        case 0x200000:
            rom_bank = (byte & 0x1F);
            high_bank = (ram_bank << 5) | rom_bank;
            break;
        default:
            break;
        }
        rom_bank = rom_bank ? rom_bank : 1;
        high_bank = high_bank ? high_bank : 1;
    } else if (utility::in_range(addr, mbc1_write_map::ram_bank_select)) {
        spdlog::debug("Writing {:02X} to ram_bank_select", byte);
        ram_bank = (byte & 0x03);
    } else if (utility::in_range(addr, mbc1_write_map::mode_select)) {
        spdlog::debug("Writing {:02X} to mode_select", byte);
        mode = (byte & 0x01);
    } else if (utility::in_range(addr, mbc1_write_map::ram)) {
        spdlog::debug("Writing {:02X} to RAM address {:X}, but why?", byte, addr);
        if (ram_enabled) {
            u32 effective_addr = addr;
            if (cart->header.ram_size == 0x800 || cart->header.ram_size == 0x2000) {
                effective_addr = (addr - 0xA000) % cart->header.ram_size;
            } else if (cart->header.ram_size == 0x8000) {
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
