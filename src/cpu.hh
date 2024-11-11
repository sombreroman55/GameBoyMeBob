#pragma once

#include "interrupts.hh"
#include "mmu.hh"
#include "registers.hh"
#include "utils/util.hh"

namespace gameboymebob {
class Cpu {
private:
    void inc(u8* r);
    void dec(u8* r);

    // 8-bit ALU stuff
    void add(u8 val);
    void adc(u8 val);
    void sub(u8 val);
    void sbc(u8 val);

    void _and(u8 val);
    void _xor(u8 val);
    void _or(u8 val);
    void cp(u8 val);

    // 16-bit stuff
    void add(u16* dest, i8 src);
    void add(u16* dest, u16 src);

    // Misc stuff
    void daa();
    void ldhl(i8 val);
    void unimplemented();

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

    u32 handle_interrupt(u8 i);

public:
    Registers* reg = nullptr;
    InterruptController* interrupts = nullptr;
    Mmu* mem = nullptr;

    bool ime = false;
    bool pending_ime = false;
    bool halted = false;
    bool halt_bug = false;
    bool stopped = false;

    Cpu(Mmu* mmu, InterruptController* ic);
    ~Cpu();

    u32 step(void);
    u32 execute(u8 opcode);
    u32 execute_cb(u8 opcode);
};
};
