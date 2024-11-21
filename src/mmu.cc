#include "mmu.hh"
#include "cartridge.hh"
#include "joypad.hh"
#include "ppu.hh"
#include "serial.hh"
#include "timer.hh"
#include <unistd.h>

namespace gameboymebob {
Mmu::Mmu() {
    memory[0xFF03] = 0xFF;
}
Mmu::~Mmu() { }

u8 Mmu::read_io_byte(u16 addr)
{
    switch (addr) {
    case IoRegisters::joyp:
        return joypad->read_button_register();
    default:
        // No special handling, just return the byte
        return memory[addr];
    }
}

void Mmu::write_io_byte(u16 addr, u8 byte)
{
    switch (addr) {
    case IoRegisters::joyp:
        joypad->write_button_register(byte);
        break;
    case IoRegisters::sc:
        // For Blargg's unit tests
        if (byte == 0x81) {
            printf("%c", memory[IoRegisters::sb]);
        }
        break;
    case IoRegisters::dma:
        // TODO: This completely ignores proper timing
        for (int i = 0; i < 160; i++) {
            ppu->write_oam_ram_byte(i, read_byte((byte << 8)+i));
        }
        break;
    case IoRegisters::bgp:
        ppu->write_bg_palette(byte);
        memory[addr] = byte; // still write it back to memory too
        break;
    case IoRegisters::obp0:
        ppu->write_obj0_palette(byte);
        memory[addr] = byte; // still write it back to memory too
        break;
    case IoRegisters::obp1:
        ppu->write_obj1_palette(byte);
        memory[addr] = byte; // still write it back to memory too
        break;
    case IoRegisters::div:
        timer->reset_div();
        break;
    default:
        // No special handling, write the byte
        memory[addr] = byte;
        break;
    }
}

void Mmu::map_cartridge(Cartridge* c)
{
    cart = c;
}

void Mmu::map_joyad(Joypad* jp)
{
    joypad = jp;
}

void Mmu::map_ppu(Ppu* p)
{
    ppu = p;
}

void Mmu::map_serial(SerialController* ser)
{
    serial = ser;
}

void Mmu::map_timer(Timer* tm)
{
    timer = tm;
}

u8 Mmu::read_byte(u16 addr)
{
    if (utility::in_range(addr, memory_map::rom_bank0)) {
        return cart->read_byte(addr);
    }

    if (utility::in_range(addr, memory_map::rom_bankN)) {
        return cart->read_byte(addr);
    }

    if (utility::in_range(addr, memory_map::video_ram)) {
        u16 effective_addr = addr - memory_map::video_ram.first;
        return ppu->read_vram_byte(effective_addr);
    }

    if (utility::in_range(addr, memory_map::ext_ram)) {
        return cart->read_byte(addr);
    }

    if (utility::in_range(addr, memory_map::work_ram0)) {
    }

    if (utility::in_range(addr, memory_map::work_ramN)) {
    }

    if (utility::in_range(addr, memory_map::echo_ram)) {
    }

    if (utility::in_range(addr, memory_map::oam_ram)) {
        u16 effective_addr = addr - memory_map::oam_ram.first;
        return ppu->read_oam_ram_byte(effective_addr);
    }

    if (utility::in_range(addr, memory_map::prohibited)) {
    }

    if (utility::in_range(addr, memory_map::io_region)) {
        return read_io_byte(addr);
    }

    if (utility::in_range(addr, memory_map::high_ram)) {
    }

    return memory[addr];
}

u16 Mmu::read_word(u16 addr)
{
    return (read_byte(addr + 1) << 8) | read_byte(addr);
}

void Mmu::write_byte(u16 addr, u8 byte)
{
    if (utility::in_range(addr, memory_map::rom_bank0)) {
        cart->write_byte(addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::rom_bankN)) {
        cart->write_byte(addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::video_ram)) {
        u16 effective_addr = addr - memory_map::video_ram.first;
        ppu->write_vram_byte(effective_addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::ext_ram)) {
        cart->write_byte(addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::work_ram0)) {
    }

    if (utility::in_range(addr, memory_map::work_ramN)) {
    }

    if (utility::in_range(addr, memory_map::echo_ram)) {
    }

    if (utility::in_range(addr, memory_map::oam_ram)) {
        u16 effective_addr = addr - memory_map::oam_ram.first;
        ppu->write_oam_ram_byte(effective_addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::prohibited)) {
    }

    if (utility::in_range(addr, memory_map::io_region)) {
        write_io_byte(addr, byte);
        return;
    }

    if (utility::in_range(addr, memory_map::high_ram)) {
    }

    memory[addr] = byte;
}

void Mmu::write_word(u16 addr, u16 word)
{
    write_byte(addr, word & 0xFF);
    write_byte(addr + 1, (word >> 8) & 0xFF);
}

void Mmu::push_stack(u16* stack, u16 word)
{
    *stack -= 2;
    write_word(*stack, word);
}

u16 Mmu::pop_stack(u16* stack)
{
    u16 value = read_word(*stack);
    *stack += 2;
    return value;
}

};
