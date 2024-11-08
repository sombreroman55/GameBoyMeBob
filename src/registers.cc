#include "registers.hh"

namespace gameboymebob {

Registers::Registers()
{
    a = 0x01;
    f = 0x80;
    b = 0x00;
    c = 0x13;
    d = 0x00;
    e = 0xD8;
    h = 0x01;
    l = 0x4D;
    sp = 0xFFFE;
    pc = 0x0100;
}

void Registers::print_registers(void)
{
    spdlog::info("a: {:02X} | f: {:02X} | af: {:04X}", a, f, af);
    spdlog::info("b: {:02X} | c: {:02X} | bc: {:04X}", b, c, bc);
    spdlog::info("d: {:02X} | e: {:02X} | de: {:04X}", d, e, de);
    spdlog::info("j: {:02X} | l: {:02X} | hl: {:04X}", h, l, hl);
    spdlog::info("sp: {:04X}", sp);
    spdlog::info("pc: {:04X}", pc);
}

void Registers::print_flags(void)
{
    spdlog::info("z: {} | n: {} | h: {} | c: {}",
            flag_is_set(Flag::ZERO),
            flag_is_set(Flag::NEG),
            flag_is_set(Flag::HALF),
            flag_is_set(Flag::CARRY));
}

bool Registers::flag_is_set(Flag flag)
{
    return (this->f & (1 << static_cast<u8>(flag)));
}

void Registers::set_flag(Flag flag, bool set)
{
    if (set) {
        this->f |= (1 << static_cast<u8>(flag));
    } else {
        this->f &= ~(1 << static_cast<u8>(flag));
    }
}

};
