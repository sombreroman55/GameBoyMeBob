#pragma once

#include "mmu.hh"
#include "registers.hh"
#include "utils/util.hh"

namespace gameboymebob {
class Cpu {
public:
    Registers* reg;
    Mmu* mem = nullptr;

    bool halted  = false;
    bool stopped = false;

    Cpu(); // standalone constructor for testing
    Cpu(Mmu* mmu);
    ~Cpu();

    u32 step(void);
    u32 execute(u8 opcode);
    u32 execute_cb(u8 opcode);
};
};
