#pragma once

#include "cartridge.hh"
#include "cpu.hh"
#include "interrupts.hh"
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
    Mmu* mem = nullptr;
    Ppu* ppu = nullptr;
    SerialController* serial = nullptr;
    Timer* timer = nullptr;

public:
    GameBoy();
    ~GameBoy();

    void run(void);
    void insert_cartridge(Cartridge* c);
};
};
