#pragma once

#include "mmu.hh"
#include "registers.hh"
#include "utils/util.hh"

namespace gameboymebob {
class Cpu {
private:
    void inc(u8* r);
    void dec(u8* r);

    // CB opcodes
    void rlc(u8* r);
    void rrc(u8* r);
    void rl(u8* r);
    void rr(u8* r);
    void sla(u8* r);
    void sra(u8* r);
    void swap(u8* r);
    void srl(u8* r);
    void bit(u8 r, u8 pos);

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
