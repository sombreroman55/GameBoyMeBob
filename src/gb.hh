#pragma once

#include "cartridge.hh"
#include "cpu.hh"
#include "interrupts.hh"
#include "joypad.hh"
#include "mmu.hh"
#include "ppu.hh"
#include "serial.hh"
#include "timer.hh"

namespace gameboymebob {
class GameBoy {
private:
    Cpu* cpu = nullptr;
    Cartridge* cart = nullptr;
    InterruptController* interrupts = nullptr;
    Joypad* joypad = nullptr;
    Mmu* mem = nullptr;
    Ppu* ppu = nullptr;
    SerialController* serial = nullptr;
    Timer* timer = nullptr;

public:
    GameBoy();
    ~GameBoy();

    void run(void);
    void insert_cartridge(Cartridge* c);

    bool frame_ready(void);
    u8* get_frame(void);

    void press_button(Button button);
    void release_button(Button button);

    // Debug API
    u8* get_memory(void);
    u8* get_vram(void);
    u8* get_oam_ram(void);
    Cpu* get_cpu(void);
};
};
