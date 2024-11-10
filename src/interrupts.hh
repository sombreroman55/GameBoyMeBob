#pragma once

#include "mmu.hh"

namespace gameboymebob {

enum class Interrupt {
    VBLANK,
    LCDSTAT,
    TIMER,
    SERIAL,
    JOYPAD
};

class InterruptController {
private:
    Mmu* mem;
    u8* _if;
    u8* _ie;

public:
    InterruptController(Mmu* mmu);
    ~InterruptController();

    void set_interrupt(Interrupt i);
    void clear_interrupt(Interrupt i);

    void enable_interrupt(Interrupt i);
    void disable_interrupt(Interrupt i);

    u8 get_flagged_interrupts(void);
    u8 get_enabled_interrupts(void);
};
};
