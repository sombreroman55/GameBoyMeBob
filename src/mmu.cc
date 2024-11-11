#include "mmu.hh"
#include "cartridge.hh"
#include "serial.hh"
#include "timer.hh"
#include <unistd.h>

namespace gameboymebob {
Mmu::Mmu() { }
Mmu::~Mmu() { }

bool Mmu::in_range(u16 addr, MemRange range)
{
    return range.first <= addr && addr <= range.second;
}

u8 Mmu::read_io_byte(u16 addr)
{
    switch (addr) {
    default:
        // No special handling, just return the byte
        return memory[addr];
    }
}

void Mmu::write_io_byte(u16 addr, u8 byte)
{
    switch (addr) {
    case IoRegisters::sc:
        // For Blargg's unit tests
        if (byte == 0x81) {
            printf("%c", memory[IoRegisters::sb]);
        }
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
    if (in_range(addr, memory_map::rom_bank0)) {
        return cart->read_byte(addr);
    }

    if (in_range(addr, memory_map::rom_bankN)) {
        return cart->read_byte(addr);
    }

    if (in_range(addr, memory_map::video_ram)) {
    }

    if (in_range(addr, memory_map::ext_ram)) {
    }

    if (in_range(addr, memory_map::work_ram0)) {
    }

    if (in_range(addr, memory_map::work_ramN)) {
    }

    if (in_range(addr, memory_map::echo_ram)) {
    }

    if (in_range(addr, memory_map::oam_ram)) {
    }

    if (in_range(addr, memory_map::prohibited)) {
    }

    if (in_range(addr, memory_map::io_region)) {
        return read_io_byte(addr);
    }

    if (in_range(addr, memory_map::high_ram)) {
    }

    return memory[addr];
}

u16 Mmu::read_word(u16 addr)
{
    return (memory[addr + 1] << 8) | memory[addr];
}

void Mmu::write_byte(u16 addr, u8 byte)
{
    if (in_range(addr, memory_map::rom_bank0)) {
        cart->write_byte(addr, byte);
        return;
    }

    if (in_range(addr, memory_map::rom_bankN)) {
        cart->write_byte(addr, byte);
        return;
    }

    if (in_range(addr, memory_map::video_ram)) {
    }

    if (in_range(addr, memory_map::ext_ram)) {
    }

    if (in_range(addr, memory_map::work_ram0)) {
    }

    if (in_range(addr, memory_map::work_ramN)) {
    }

    if (in_range(addr, memory_map::echo_ram)) {
    }

    if (in_range(addr, memory_map::oam_ram)) {
    }

    if (in_range(addr, memory_map::prohibited)) {
    }

    if (in_range(addr, memory_map::io_region)) {
        write_io_byte(addr, byte);
        return;
    }

    if (in_range(addr, memory_map::high_ram)) {
    }

    memory[addr] = byte;
}

void Mmu::write_word(u16 addr, u16 word)
{
    memory[addr] = word & 0xFF;
    memory[addr + 1] = (word >> 8) & 0xFF;
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
