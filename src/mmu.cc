#include "mmu.hh"

namespace gameboymebob {
Mmu::Mmu() { }
Mmu::~Mmu() { }

u8 Mmu::read_byte(u16 addr)
{
    return memory[addr];
}

u16 Mmu::read_word(u16 addr)
{
    return (memory[addr+1] << 8) | memory[addr];
}

void Mmu::write_byte(u16 addr, u8 byte)
{
    memory[addr] = byte;
}

void Mmu::write_word(u16 addr, u16 word)
{
    memory[addr] = word & 0xFF;
    memory[addr + 1] = (word >> 8) & 0xFF;
}

};
