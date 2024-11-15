#pragma once

#include "interrupts.hh"
#include "mmu.hh"
#include "utils/util.hh"

namespace gameboymebob {
    // The buttons a user program would "press"
    enum class Button : u8 {
        Right,
        Left,
        Up,
        Down,
        A,
        B,
        Select,
        Start,
        Invalid
    };

    class Joypad {
    private:
        Mmu* mem;
        InterruptController* interrupts;
        u8* joyp;
        u8 button_gates;

        bool opposing_directions_pressed(Button button);
        u8 read_buttons(void);
        u8 read_directions(void);

    public:
        Joypad(Mmu* mmu, InterruptController* ic);
        void write_button_register(u8 buttons);
        u8 read_button_register(void);

        // "External" API
        void press_button(Button button);
        void release_button(Button button);
    };
};
