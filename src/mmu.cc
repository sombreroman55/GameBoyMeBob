#include "mmu.hh"
#include "cartridge.hh"
#include "serial.hh"
#include <unistd.h>

namespace gameboymebob {
Mmu::Mmu() { }
Mmu::~Mmu() { }

void Mmu::write_io_byte(u16 addr, u8 byte)
{
    switch (addr) {
    case 0xFF02:
        // For Blargg's unit tests
        if (byte == 0x81) {
            printf("%c", memory[0xFF01]);
        }
        break;
    default:
        // Nothing special, just write the byte
        memory[addr] = byte;
        break;
    }
}

void Mmu::map_serial(SerialController* ser)
{
    serial = ser;
}

u8 Mmu::read_byte(u16 addr)
{
    return memory[addr];
}

u16 Mmu::read_word(u16 addr)
{
    return (memory[addr + 1] << 8) | memory[addr];
}

void Mmu::write_byte(u16 addr, u8 byte)
{
    if (0xFF00 <= addr && addr <= 0xFF7F) {
        write_io_byte(addr, byte);
        return;
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

void Mmu::load_cartridge(Cartridge* cart)
{
    for (int i = 0; i < 0x8000; i++) {
        memory[i] = cart->rom_data[i];
    }
}

};
