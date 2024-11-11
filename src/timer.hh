#pragma once

#include "interrupts.hh"
#include "mmu.hh"

namespace gameboymebob {
class Timer {
private:
    Mmu* mem;
    InterruptController* interrupts;
    u8* div;
    u8* tima;
    u8* tma;
    u8* tac;

    u32 counter = 0;

    bool timer_is_enabled(void);


public:
    Timer(Mmu* mmu, InterruptController* ic);
    ~Timer();

    void reset_div(void);
    void tick(u32 cycles);
};
};
