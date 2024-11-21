#include "timer.hh"

namespace gameboymebob {
Timer::Timer(Mmu* mmu, InterruptController* ic)
    : mem(mmu)
    , interrupts(ic)
    , div(&mem->memory[IoRegisters::div])
    , tima(&mem->memory[IoRegisters::tima])
    , tma(&mem->memory[IoRegisters::tma])
    , tac(&mem->memory[IoRegisters::tac])
{
    *div = 0xAB;
    *tima = 0x00;
    *tma = 0x00;
    *tac = 0xF8;
    counter = (*div << 6);
    mem->map_timer(this);
}

Timer::~Timer() { }

void Timer::reset_div(void)
{
    counter = 0;
    *div = (counter >> 6);
}

void Timer::tick(u32 cycles)
{
    const int counter_shift[4] = { 8, 2, 4, 6 };

    u32 last_counter = counter;
    counter += cycles;
    counter %= (1 << 20);
    *div = (counter >> 6);

    bool timer_enabled = utility::bitwise::is_bit_set(tac, 2);
    if (timer_enabled) {
        int selected_shift = (*tac & 0x03);
        int shift = counter_shift[selected_shift];

        u32 last_ticks = (last_counter >> shift);
        u32 current_ticks = (counter >> shift);

        u32 ticks = current_ticks - last_ticks;
        u16 updated_tima = *tima + ticks;

        if (updated_tima > 0xFF) {
            interrupts->set_interrupt(Interrupt::TIMER);
            *tima = *tma;
            *tima += (updated_tima >> 8);
        } else {
            *tima = (updated_tima & 0xFF);
        }
    }
}

};
