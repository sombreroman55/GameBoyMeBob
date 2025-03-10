#include "interrupts.hh"
#include "utils/util.hh"

namespace gameboymebob {
InterruptController::InterruptController(Mmu* mmu)
    : mem(mmu)
    , _if(&mem->memory[IoRegisters::_if])
    , _ie(&mem->memory[IoRegisters::_ie])
{
    *_if = 0xE1;
    *_ie = 0x00;
}

InterruptController::~InterruptController() {}

void InterruptController::set_interrupt(Interrupt i)
{
    u8 intr = static_cast<u8>(i);
    utility::bitwise::set_bit(_if, intr);
}

void InterruptController::clear_interrupt(Interrupt i)
{
    u8 intr = static_cast<u8>(i);
    utility::bitwise::reset_bit(_if, intr);
}

void InterruptController::enable_interrupt(Interrupt i)
{
    u8 intr = static_cast<u8>(i);
    utility::bitwise::set_bit(_ie, intr);
}

void InterruptController::disable_interrupt(Interrupt i)
{
    u8 intr = static_cast<u8>(i);
    utility::bitwise::reset_bit(_ie, intr);
}

u8 InterruptController::get_flagged_interrupts(void)
{
    return *_if;
}

u8 InterruptController::get_enabled_interrupts(void)
{
    return *_ie;
}

};
