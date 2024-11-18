#include "cpu.hh"
#include "instructions.hh"
#include "interrupts.hh"
#include "utils/util.hh"

namespace gameboymebob {
Cpu::Cpu(Mmu* mmu, InterruptController* ic)
    : mem(mmu)
    , interrupts(ic)
{
    reg = new Registers();
}

Cpu::~Cpu()
{
    mem = nullptr;
    interrupts = nullptr;
    delete reg;
}

u32 Cpu::handle_interrupt(u8 i)
{
    constexpr u32 interrupt_ticks = 5;
    static const u16 interrupt_vector[5] = { 0x0040, 0x0048, 0x0050, 0x0058, 0x0060 };
    Interrupt intr = static_cast<Interrupt>(i);
    ime = false;
    halted = false;
    interrupts->clear_interrupt(intr);

    // Call interrupt handler
    mem->push_stack(&reg->sp, reg->pc);
    reg->pc = interrupt_vector[i];

    // The whole process takes 5 mcycles
    return interrupt_ticks;
}

u32 Cpu::step(void)
{
    u32 cycles_consumed = 1;
    // if (stopped) {
        // return cycles_consumed;
    // }
    // TBD: Do we want to "implement" stopping?

    // Check for interrupts
    u8 enabled_interrupts = interrupts->get_enabled_interrupts();
    u8 flagged_interrupts = interrupts->get_flagged_interrupts();
    u8 pending_interrupts = (enabled_interrupts & flagged_interrupts & 0x1F);

    if (halted && pending_interrupts) {
        halted = false;
    }

    bool effective_ime = ime;
    if (effective_ime && pending_interrupts) {
        for (u8 i = 0; i < 5; i++) {
            u8 intr = (1 << i);
            if (pending_interrupts & intr) {
                return handle_interrupt(i);
            }
        }
    }

    if (halted) {
        return cycles_consumed;
    }

    if (pending_ime) {
        ime = true;
        pending_ime = false;
    }

    u8 opcode = mem->read_byte(reg->pc++);
    if (opcode == 0xCB) {
        // CB prefix
        u8 cb_opcode = mem->read_byte(reg->pc++);
        cycles_consumed = execute_cb(cb_opcode);
    } else {
        /*
        printf("%02X", opcode);
        for (int i = 0; i < instructions[opcode].size-1; i++) {
            printf(" %02X", mem->read_byte(reg->pc+i));
        }
        printf("\n");
        */
        cycles_consumed = execute(opcode);
    }
    // reg->print_registers();

    if (halt_bug) {
        reg->pc--;
        halt_bug = false;
    }

    return cycles_consumed;
}

// Normal opcodes
u32 Cpu::execute(u8 opcode)
{
    u8 n;
    u16 nn;
    i8 i;
    bool branch_taken = false;
    switch (opcode) {
    case 0x00: // nop
        break;

    case 0x01: // ld bc, u16
        reg->bc = mem->read_word(reg->pc);
        reg->pc += 2;
        break;

    case 0x02: // ld [bc], a
        mem->write_byte(reg->bc, reg->a);
        break;

    case 0x03: // inc bc
        reg->bc++;
        break;

    case 0x04: // inc b
        inc(&reg->b);
        break;

    case 0x05: // dec b
        dec(&reg->b);
        break;

    case 0x06: // ld b, u8
        reg->b = mem->read_byte(reg->pc++);
        break;

    case 0x07: // rlca
        rlc(&reg->a);
        reg->set_flag(Flag::ZERO, false);
        break;

    case 0x08: // ld [u16], sp
        mem->write_word(mem->read_word(reg->pc), reg->sp);
        reg->pc += 2;
        break;

    case 0x09: // add hl, bc
        add(&reg->hl, reg->bc);
        break;

    case 0x0A: // ld a, [bc]
        reg->a = mem->read_byte(reg->bc);
        break;

    case 0x0B: // dec bc
        reg->bc--;
        break;

    case 0x0C: // inc c
        inc(&reg->c);
        break;

    case 0x0D: // dec c
        dec(&reg->c);
        break;

    case 0x0E: // ld b, u8
        reg->c = mem->read_byte(reg->pc++);
        break;

    case 0x0F: // rrca
        rrc(&reg->a);
        reg->set_flag(Flag::ZERO, false);
        break;

    case 0x10: // stop
        // stopped = true; // TBD: Do we want to "implement" stopping
        break;

    case 0x11: // ld de, u16
        reg->de = mem->read_word(reg->pc);
        reg->pc += 2;
        break;

    case 0x12: // ld [de], a
        mem->write_byte(reg->de, reg->a);
        break;

    case 0x13: // inc de
        reg->de++;
        break;

    case 0x14: // inc d
        inc(&reg->d);
        break;

    case 0x15: // dec d
        dec(&reg->d);
        break;

    case 0x16: // ld b, u8
        reg->d = mem->read_byte(reg->pc++);
        break;

    case 0x17: // rla
        rl(&reg->a);
        reg->set_flag(Flag::ZERO, false);
        break;

    case 0x18: // jr s8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        reg->pc += i;
        break;

    case 0x19: // add hl, de
        add(&reg->hl, reg->de);
        break;

    case 0x1A: // ld a, [de]
        reg->a = mem->read_byte(reg->de);
        break;

    case 0x1B: // dec de
        reg->de--;
        break;

    case 0x1C: // inc e
        inc(&reg->e);
        break;

    case 0x1D: // dec e
        dec(&reg->e);
        break;

    case 0x1E: // ld e, u8
        reg->e = mem->read_byte(reg->pc++);
        break;

    case 0x1F: // rra
        rr(&reg->a);
        reg->set_flag(Flag::ZERO, false);
        break;

    case 0x20: // jr nz, i8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        if (!reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc += i;
        }
        break;

    case 0x21: // ld hl, u16
        reg->hl = mem->read_word(reg->pc);
        reg->pc += 2;
        break;

    case 0x22: // ld [hl+], a
        mem->write_byte(reg->hl++, reg->a);
        break;

    case 0x23: // inc hl
        reg->hl++;
        break;

    case 0x24: // inc h
        inc(&reg->h);
        break;

    case 0x25: // dec h
        dec(&reg->h);
        break;

    case 0x26: // ld h, u8
        reg->h = mem->read_byte(reg->pc++);
        break;

    case 0x27: // daa
        daa();
        break;

    case 0x28: // jr z, s8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        if (reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc += i;
        }
        break;

    case 0x29: // add hl, hl
        add(&reg->hl, reg->hl);
        break;

    case 0x2A: // ld a, [hl+]
        reg->a = mem->read_byte(reg->hl++);
        break;

    case 0x2B: // dec hl
        reg->hl--;
        break;

    case 0x2C: // inc l
        inc(&reg->l);
        break;

    case 0x2D: // dec l
        dec(&reg->l);
        break;

    case 0x2E: // ld l, u8
        reg->l = mem->read_byte(reg->pc++);
        break;

    case 0x2F: // cpl
        reg->a = ~reg->a;
        reg->set_flag(Flag::NEG, true);
        reg->set_flag(Flag::HALF, true);
        break;

    case 0x30: // jr nc, s8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        if (!reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc += i;
        }
        break;

    case 0x31: // ld sp, u16
        reg->sp = mem->read_word(reg->pc);
        reg->pc += 2;
        break;

    case 0x32: // ld [hl-], a
        mem->write_byte(reg->hl--, reg->a);
        break;

    case 0x33: // inc sp
        reg->sp++;
        break;

    case 0x34: // inc [hl]
        n = mem->read_byte(reg->hl);
        inc(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x35: // dec [hl]
        n = mem->read_byte(reg->hl);
        dec(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x36: // ld [hl], u8
        mem->write_byte(reg->hl, mem->read_byte(reg->pc++));
        break;

    case 0x37: // scf
        reg->set_flag(Flag::CARRY, true);
        reg->set_flag(Flag::NEG, false);
        reg->set_flag(Flag::HALF, false);
        break;

    case 0x38: // jr c, s8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        if (reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc += i;
        }
        break;

    case 0x39: // add hl, sp
        add(&reg->hl, reg->sp);
        break;

    case 0x3A: // ld a, [hl-]
        reg->a = mem->read_byte(reg->hl--);
        break;

    case 0x3B: // dec sp
        reg->sp--;
        break;

    case 0x3C: // inc a
        inc(&reg->a);
        break;

    case 0x3D: // dec a
        dec(&reg->a);
        break;

    case 0x3E: // ld a, u8
        reg->a = mem->read_byte(reg->pc++);
        break;

    case 0x3F: // ccf
        reg->set_flag(Flag::CARRY, !reg->flag_is_set(Flag::CARRY));
        reg->set_flag(Flag::NEG, false);
        reg->set_flag(Flag::HALF, false);
        break;

    case 0x40: // ld b, b
        reg->b = reg->b;
        break;

    case 0x41: // ld b, c
        reg->b = reg->c;
        break;

    case 0x42: // ld b, d
        reg->b = reg->d;
        break;

    case 0x43: // ld b, e
        reg->b = reg->e;
        break;

    case 0x44: // ld b, h
        reg->b = reg->h;
        break;

    case 0x45: // ld b, l
        reg->b = reg->l;
        break;

    case 0x46: // ld b, [hl]
        reg->b = mem->read_byte(reg->hl);
        break;

    case 0x47: // ld b, a
        reg->b = reg->a;
        break;

    case 0x48: // ld c, b
        reg->c = reg->b;
        break;

    case 0x49: // ld c, c
        reg->c = reg->c;
        break;

    case 0x4A: // ld c, d
        reg->c = reg->d;
        break;

    case 0x4B: // ld c, e
        reg->c = reg->e;
        break;

    case 0x4C: // ld c, h
        reg->c = reg->h;
        break;

    case 0x4D: // ld c, l
        reg->c = reg->l;
        break;

    case 0x4E: // ld c, [hl]
        reg->c = mem->read_byte(reg->hl);
        break;

    case 0x4F: // ld c, a
        reg->c = reg->a;
        break;

    case 0x50: // ld d, b
        reg->d = reg->b;
        break;

    case 0x51: // ld d, c
        reg->d = reg->c;
        break;

    case 0x52: // ld d, d
        reg->d = reg->d;
        break;

    case 0x53: // ld d, e
        reg->d = reg->e;
        break;

    case 0x54: // ld d, h
        reg->d = reg->h;
        break;

    case 0x55: // ld d, l
        reg->d = reg->l;
        break;

    case 0x56: // ld d, [hl]
        reg->d = mem->read_byte(reg->hl);
        break;

    case 0x57: // ld d, a
        reg->d = reg->a;
        break;

    case 0x58: // ld e, b
        reg->e = reg->b;
        break;

    case 0x59: // ld e, c
        reg->e = reg->c;
        break;

    case 0x5A: // ld e, d
        reg->e = reg->d;
        break;

    case 0x5B: // ld e, e
        reg->e = reg->e;
        break;

    case 0x5C: // ld e, h
        reg->e = reg->h;
        break;

    case 0x5D: // ld e, l
        reg->e = reg->l;
        break;

    case 0x5E: // ld e, [hl]
        reg->e = mem->read_byte(reg->hl);
        break;

    case 0x5F: // ld e, a
        reg->e = reg->a;
        break;

    case 0x60: // ld h, b
        reg->h = reg->b;
        break;

    case 0x61: // ld h, c
        reg->h = reg->c;
        break;

    case 0x62: // ld h, d
        reg->h = reg->d;
        break;

    case 0x63: // ld h, e
        reg->h = reg->e;
        break;

    case 0x64: // ld h, h
        reg->h = reg->h;
        break;

    case 0x65: // ld h, l
        reg->h = reg->l;
        break;

    case 0x66: // ld h, [hl]
        reg->h = mem->read_byte(reg->hl);
        break;

    case 0x67: // ld h, a
        reg->h = reg->a;
        break;

    case 0x68: // ld l, b
        reg->l = reg->b;
        break;

    case 0x69: // ld l, c
        reg->l = reg->c;
        break;

    case 0x6A: // ld l, d
        reg->l = reg->d;
        break;

    case 0x6B: // ld l, e
        reg->l = reg->e;
        break;

    case 0x6C: // ld l, h
        reg->l = reg->h;
        break;

    case 0x6D: // ld l, l
        reg->l = reg->l;
        break;

    case 0x6E: // ld l, [hl]
        reg->l = mem->read_byte(reg->hl);
        break;

    case 0x6F: // ld l, a
        reg->l = reg->a;
        break;

    case 0x70: // ld [hl], b
        mem->write_byte(reg->hl, reg->b);
        break;

    case 0x71: // ld [hl], c
        mem->write_byte(reg->hl, reg->c);
        break;

    case 0x72: // ld [hl], d
        mem->write_byte(reg->hl, reg->d);
        break;

    case 0x73: // ld [hl], e
        mem->write_byte(reg->hl, reg->e);
        break;

    case 0x74: // ld [hl], h
        mem->write_byte(reg->hl, reg->h);
        break;

    case 0x75: // ld [hl], l
        mem->write_byte(reg->hl, reg->l);
        break;

    case 0x76: // halt
        if ((interrupts->get_flagged_interrupts() & interrupts->get_enabled_interrupts()) != 0) {
            if (ime) {
                halted = false;
                reg->pc--;
            } else {
                halted = false;
                halt_bug = true;
            }
        } else {
            halted = true;
        }
        break;

    case 0x77: // ld [hl], a
        mem->write_byte(reg->hl, reg->a);
        break;

    case 0x78: // ld a, b
        reg->a = reg->b;
        break;

    case 0x79: // ld a, c
        reg->a = reg->c;
        break;

    case 0x7A: // ld a, d
        reg->a = reg->d;
        break;

    case 0x7B: // ld a, e
        reg->a = reg->e;
        break;

    case 0x7C: // ld a, h
        reg->a = reg->h;
        break;

    case 0x7D: // ld a, l
        reg->a = reg->l;
        break;

    case 0x7E: // ld a, [hl]
        reg->a = mem->read_byte(reg->hl);
        break;

    case 0x7F: // ld a, a
        reg->a = reg->a;
        break;

    case 0x80: // add a, b
        add(reg->b);
        break;

    case 0x81: // add a, c
        add(reg->c);
        break;

    case 0x82: // add a, d
        add(reg->d);
        break;

    case 0x83: // add a, e
        add(reg->e);
        break;

    case 0x84: // add a, h
        add(reg->h);
        break;

    case 0x85: // add a, l
        add(reg->l);
        break;

    case 0x86: // add a, [hl]
        add(mem->read_byte(reg->hl));
        break;

    case 0x87: // add a, a
        add(reg->a);
        break;

    case 0x88: // adc a, b
        adc(reg->b);
        break;

    case 0x89: // adc a, c
        adc(reg->c);
        break;

    case 0x8A: // adc a, d
        adc(reg->d);
        break;

    case 0x8B: // adc a, e
        adc(reg->e);
        break;

    case 0x8C: // adc a, h
        adc(reg->h);
        break;

    case 0x8D: // adc a, l
        adc(reg->l);
        break;

    case 0x8E: // adc a, [hl]
        adc(mem->read_byte(reg->hl));
        break;

    case 0x8F: // adc a, a
        adc(reg->a);
        break;

    case 0x90: // sub a, b
        sub(reg->b);
        break;

    case 0x91: // sub a, c
        sub(reg->c);
        break;

    case 0x92: // sub a, d
        sub(reg->d);
        break;

    case 0x93: // sub a, e
        sub(reg->e);
        break;

    case 0x94: // sub a, h
        sub(reg->h);
        break;

    case 0x95: // sub a, l
        sub(reg->l);
        break;

    case 0x96: // sub a, [hl]
        sub(mem->read_byte(reg->hl));
        break;

    case 0x97: // sub a, a
        sub(reg->a);
        break;

    case 0x98: // sbc a, b
        sbc(reg->b);
        break;

    case 0x99: // sbc a, c
        sbc(reg->c);
        break;

    case 0x9A: // sbc a, d
        sbc(reg->d);
        break;

    case 0x9B: // sbc a, e
        sbc(reg->e);
        break;

    case 0x9C: // sbc a, h
        sbc(reg->h);
        break;

    case 0x9D: // sbc a, l
        sbc(reg->l);
        break;

    case 0x9E: // sbc a, [hl]
        sbc(mem->read_byte(reg->hl));
        break;

    case 0x9F: // sbc a, a
        sbc(reg->a);
        break;

    case 0xA0: // and a, b
        _and(reg->b);
        break;

    case 0xA1: // and a, c
        _and(reg->c);
        break;

    case 0xA2: // and a, d
        _and(reg->d);
        break;

    case 0xA3: // and a, e
        _and(reg->e);
        break;

    case 0xA4: // and a, h
        _and(reg->h);
        break;

    case 0xA5: // and a, l
        _and(reg->l);
        break;

    case 0xA6: // and a, [hl]
        _and(mem->read_byte(reg->hl));
        break;

    case 0xA7: // and a, a
        _and(reg->a);
        break;

    case 0xA8: // xor a, b
        _xor(reg->b);
        break;

    case 0xA9: // xor a, c
        _xor(reg->c);
        break;

    case 0xAA: // xor a, d
        _xor(reg->d);
        break;

    case 0xAB: // xor a, e
        _xor(reg->e);
        break;

    case 0xAC: // xor a, h
        _xor(reg->h);
        break;

    case 0xAD: // xor a, l
        _xor(reg->l);
        break;

    case 0xAE: // xor a, [hl]
        _xor(mem->read_byte(reg->hl));
        break;

    case 0xAF: // xor a, a
        _xor(reg->a);
        break;

    case 0xB0: // or a, b
        _or(reg->b);
        break;

    case 0xB1: // or a, c
        _or(reg->c);
        break;

    case 0xB2: // or a, d
        _or(reg->d);
        break;

    case 0xB3: // or a, e
        _or(reg->e);
        break;

    case 0xB4: // or a, h
        _or(reg->h);
        break;

    case 0xB5: // or a, l
        _or(reg->l);
        break;

    case 0xB6: // or a, [hl]
        _or(mem->read_byte(reg->hl));
        break;

    case 0xB7: // or a, a
        _or(reg->a);
        break;

    case 0xB8: // cp a, b
        cp(reg->b);
        break;

    case 0xB9: // cp a, c
        cp(reg->c);
        break;

    case 0xBA: // cp a, d
        cp(reg->d);
        break;

    case 0xBB: // cp a, e
        cp(reg->e);
        break;

    case 0xBC: // cp a, h
        cp(reg->h);
        break;

    case 0xBD: // cp a, l
        cp(reg->l);
        break;

    case 0xBE: // cp a, [hl]
        cp(mem->read_byte(reg->hl));
        break;

    case 0xBF: // cp a, a
        cp(reg->a);
        break;

    case 0xC0: // ret nz
        if (!reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc = mem->pop_stack(&reg->sp);
        }
        break;

    case 0xC1: // pop bc
        reg->bc = mem->pop_stack(&reg->sp);
        break;

    case 0xC2: // jp nz, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (!reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc = nn;
        }
        break;

    case 0xC3: // jp u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        reg->pc = nn;
        break;

    case 0xC4: // call nz, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (!reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            mem->push_stack(&reg->sp, reg->pc);
            reg->pc = nn;
        }
        break;

    case 0xC5: // push bc
        mem->push_stack(&reg->sp, reg->bc);
        break;

    case 0xC6: // add a, u8
        add(mem->read_byte(reg->pc++));
        break;

    case 0xC7: // rst 00h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0000;
        break;

    case 0xC8: // ret z
        if (reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc = mem->pop_stack(&reg->sp);
        }
        break;

    case 0xC9: // ret
        reg->pc = mem->pop_stack(&reg->sp);
        break;

    case 0xCA: // jp z, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            reg->pc = nn;
        }
        break;

    case 0xCB: // prefix cb
        // Should never get called, handled before we enter this function
        break;

    case 0xCC: // call z, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (reg->flag_is_set(Flag::ZERO)) {
            branch_taken = true;
            mem->push_stack(&reg->sp, reg->pc);
            reg->pc = nn;
        }
        break;

    case 0xCD: // call u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = nn;
        break;

    case 0xCE: // adc a, u8
        adc(mem->read_byte(reg->pc++));
        break;

    case 0xCF: // rst 08h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0008;
        break;

    case 0xD0: // ret nc
        if (!reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc = mem->pop_stack(&reg->sp);
        }
        break;

    case 0xD1: // pop de
        reg->de = mem->pop_stack(&reg->sp);
        break;

    case 0xD2: // jp nc, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (!reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc = nn;
        }
        break;

    case 0xD3: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xD4: // call nc, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (!reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            mem->push_stack(&reg->sp, reg->pc);
            reg->pc = nn;
        }
        break;

    case 0xD5: // push de
        mem->push_stack(&reg->sp, reg->de);
        break;

    case 0xD6: // sub a, u8
        sub(mem->read_byte(reg->pc++));
        break;

    case 0xD7: // rst 10h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0010;
        break;

    case 0xD8: // ret c
        if (reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc = mem->pop_stack(&reg->sp);
        }
        break;

    case 0xD9: // reti
        reg->pc = mem->pop_stack(&reg->sp);
        ime = true;
        break;

    case 0xDA: // jp c, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            reg->pc = nn;
        }
        break;

    case 0xDB: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xDC: // call c, u16
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        if (reg->flag_is_set(Flag::CARRY)) {
            branch_taken = true;
            mem->push_stack(&reg->sp, reg->pc);
            reg->pc = nn;
        }
        break;

    case 0xDD: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xDE: // sbc a, u8
        sbc(mem->read_byte(reg->pc++));
        break;

    case 0xDF: // rst 18h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0018;
        break;

    case 0xE0: // ld [FF00+u8], a
        n = mem->read_byte(reg->pc++);
        mem->write_byte(0xFF00 + n, reg->a);
        break;

    case 0xE1: // pop hl
        reg->hl = mem->pop_stack(&reg->sp);
        break;

    case 0xE2: // ld [FF00+c], a
        mem->write_byte(0xFF00 + reg->c, reg->a);
        break;

    case 0xE3: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xE4: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xE5: // push hl
        mem->push_stack(&reg->sp, reg->hl);
        break;

    case 0xE6: // and a, u8
        _and(mem->read_byte(reg->pc++));
        break;

    case 0xE7: // rst 0x20h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0020;
        break;

    case 0xE8: // add sp, i8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        add(&reg->sp, i);
        break;

    case 0xE9: // jp hl
        reg->pc = reg->hl;
        break;

    case 0xEA: // ld [u16], a
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        mem->write_byte(nn, reg->a);
        break;

    case 0xEB: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xEC: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xED: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xEE: // xor a, u8
        _xor(mem->read_byte(reg->pc++));
        break;

    case 0xEF: // rst 28h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0028;
        break;

    case 0xF0: // ld a, [FF00+u8]
        n = mem->read_byte(reg->pc++);
        reg->a = mem->read_byte(0xFF00 + n);
        break;

    case 0xF1: // pop af
        reg->af = mem->pop_stack(&reg->sp);
        reg->f &= 0xF0;
        break;

    case 0xF2: // ld a, [FF00+c]
        reg->a = mem->read_byte(0xFF00 + reg->c);
        break;

    case 0xF3: // di
        ime = false;
        break;

    case 0xF4: // NOT IMPLEMENTED
        unimplemented(opcode);
        break;

    case 0xF5: // push af
        mem->push_stack(&reg->sp, reg->af);
        break;

    case 0xF6: // or a, u8
        _or(mem->read_byte(reg->pc++));
        break;

    case 0xF7: // rst 30h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0030;
        break;

    case 0xF8: // ld hl, sp+s8
        i = static_cast<i8>(mem->read_byte(reg->pc++));
        ldhl(i);
        break;

    case 0xF9: // ld sp, hl
        reg->sp = reg->hl;
        break;

    case 0xFA: // ld a, [u16]
        nn = mem->read_word(reg->pc);
        reg->pc += 2;
        reg->a = mem->read_byte(nn);
        break;

    case 0xFB: // ei
        if (!ime && !pending_ime) {
            pending_ime = true;
        }
        break;

    case 0xFC: // should crash gb
        unimplemented(opcode);
        break;

    case 0xFD: // should crash gb
        unimplemented(opcode);
        break;

    case 0xFE: // cp a, u8
        cp(mem->read_byte(reg->pc++));
        break;

    case 0xFF: // rst 38h
        mem->push_stack(&reg->sp, reg->pc);
        reg->pc = 0x0038;
        break;

    default:
        spdlog::warn("Opcode 0x{:02X} not yet implemented", opcode);
        break;
    }
    return branch_taken ? instructions[opcode].mcycles_branched : instructions[opcode].mcycles;
}

void Cpu::inc(u8* r)
{
    reg->set_flag(Flag::HALF, (*r & 0x0F) == 0x0F);
    *r += 1;
    reg->set_flag(Flag::ZERO, !*r);
    reg->set_flag(Flag::NEG, false);
}

void Cpu::dec(u8* r)
{
    reg->set_flag(Flag::HALF, !(*r & 0x0F));
    *r -= 1;
    reg->set_flag(Flag::ZERO, !*r);
    reg->set_flag(Flag::NEG, true);
}

void Cpu::add(u8 val)
{
    uint16_t result = reg->a + val;

    reg->set_flag(Flag::CARRY, result > 0xFF);
    reg->set_flag(Flag::HALF, ((reg->a & 0x0F) + (val & 0x0F)) > 0x0F);

    reg->a = result;

    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::NEG, false);
}

void Cpu::adc(u8 val)
{
    int carry = reg->flag_is_set(Flag::CARRY) ? 1 : 0;
    int result = reg->a + val + carry;

    reg->set_flag(Flag::ZERO, !(int8_t)result);
    reg->set_flag(Flag::CARRY, result > 0xff);
    reg->set_flag(Flag::HALF, ((reg->a & 0x0F) + (val & 0x0f) + carry) > 0x0F);
    reg->set_flag(Flag::NEG, false);

    reg->a = (int8_t)(result & 0xff);
}

void Cpu::sub(u8 val)
{
    reg->set_flag(Flag::CARRY, val > reg->a);
    reg->set_flag(Flag::HALF, (val & 0x0f) > (reg->a & 0x0f));

    reg->a -= val;

    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::NEG, true);
}

void Cpu::sbc(u8 val)
{
    bool is_carry = reg->flag_is_set(Flag::CARRY);

    reg->set_flag(Flag::CARRY, (val + is_carry) > reg->a);
    reg->set_flag(Flag::HALF, ((val & 0x0f) + is_carry) > (reg->a & 0x0f));

    reg->a -= (val + is_carry);

    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::NEG, true);
}

void Cpu::_and(u8 val)
{
    reg->a &= val;
    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::HALF, true);
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::CARRY, false);
}

void Cpu::_xor(u8 val)
{
    reg->a ^= val;
    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::CARRY, false);
}

void Cpu::_or(u8 val)
{
    reg->a |= val;
    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::CARRY, false);
}

void Cpu::cp(u8 val)
{
    uint8_t temp_val = reg->a;
    reg->set_flag(Flag::CARRY, val > temp_val);
    reg->set_flag(Flag::HALF, (val & 0x0f) > (temp_val & 0x0f));

    temp_val -= val;

    reg->set_flag(Flag::ZERO, !temp_val);
    reg->set_flag(Flag::NEG, true);
}

void Cpu::add(u16* dest, i8 src)
{
    u16 result = *dest + src;

    reg->set_flag(Flag::CARRY, ((reg->sp ^ src ^ (result & 0xFFFF)) & 0x100) == 0x100);
    reg->set_flag(Flag::HALF, ((reg->sp ^ src ^ (result & 0xFFFF)) & 0x10) == 0x10);

    *dest = result & 0xFFFF;

    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::ZERO, false);
}

void Cpu::add(u16* dest, u16 src)
{
    u32 result = *dest + src;

    reg->set_flag(Flag::CARRY, result > 0xFFFF);
    reg->set_flag(Flag::HALF, ((*dest & 0x0FFF) + (src & 0x0FFF)) > 0x0FFF);

    *dest = (u16)result;

    reg->set_flag(Flag::NEG, false);
}

void Cpu::daa(void)
{
    u16 value = reg->a;

    if (reg->flag_is_set(Flag::NEG)) {
        if (reg->flag_is_set(Flag::CARRY)) {
            value -= 0x60;
        }

        if (reg->flag_is_set(Flag::HALF)) {
            value -= 0x6;
        }
    } else {
        if (reg->flag_is_set(Flag::CARRY) || value > 0x99) {
            value += 0x60;
            reg->set_flag(Flag::CARRY, true);
        }

        if (reg->flag_is_set(Flag::HALF) || (value & 0xF) > 0x9) {
            value += 0x6;
        }
    }
    reg->a = value;

    reg->set_flag(Flag::ZERO, !reg->a);
    reg->set_flag(Flag::HALF, false);
}

void Cpu::ldhl(i8 val)
{
    uint16_t result = reg->sp + val;

    reg->set_flag(Flag::CARRY, ((reg->sp ^ val ^ result) & 0x100) == 0x100);
    reg->set_flag(Flag::HALF, ((reg->sp ^ val ^ result) & 0x10) == 0x10);

    reg->hl = result;

    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::ZERO, false);
}

void Cpu::unimplemented(u8 opcode)
{
    spdlog::error("Opcode {:02X} is unimplemented on the Game Boy hardware!", opcode);
}

// CB opcodes
u32 Cpu::execute_cb(u8 opcode)
{
    u8 n = 0;
    spdlog::trace("Executing CB opcode {:02X}...", opcode);
    switch (opcode) {
    case 0x00:
        rlc(&reg->b);
        break;

    case 0x01:
        rlc(&reg->c);
        break;

    case 0x02:
        rlc(&reg->d);
        break;

    case 0x03:
        rlc(&reg->e);
        break;

    case 0x04:
        rlc(&reg->h);
        break;

    case 0x05:
        rlc(&reg->l);
        break;

    case 0x06:
        n = mem->read_byte(reg->hl);
        rlc(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x07:
        rlc(&reg->a);
        break;

    case 0x08:
        rrc(&reg->b);
        break;

    case 0x09:
        rrc(&reg->c);
        break;

    case 0x0A:
        rrc(&reg->d);
        break;

    case 0x0B:
        rrc(&reg->e);
        break;

    case 0x0C:
        rrc(&reg->h);
        break;

    case 0x0D:
        rrc(&reg->l);
        break;

    case 0x0E:
        n = mem->read_byte(reg->hl);
        rrc(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x0F:
        rrc(&reg->a);
        break;

    case 0x10:
        rl(&reg->b);
        break;

    case 0x11:
        rl(&reg->c);
        break;

    case 0x12:
        rl(&reg->d);
        break;

    case 0x13:
        rl(&reg->e);
        break;

    case 0x14:
        rl(&reg->h);
        break;

    case 0x15:
        rl(&reg->l);
        break;

    case 0x16:
        n = mem->read_byte(reg->hl);
        rl(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x17:
        rl(&reg->a);
        break;

    case 0x18:
        rr(&reg->b);
        break;

    case 0x19:
        rr(&reg->c);
        break;

    case 0x1A:
        rr(&reg->d);
        break;

    case 0x1B:
        rr(&reg->e);
        break;

    case 0x1C:
        rr(&reg->h);
        break;

    case 0x1D:
        rr(&reg->l);
        break;

    case 0x1E:
        n = mem->read_byte(reg->hl);
        rr(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x1F:
        rr(&reg->a);
        break;

    case 0x20:
        sla(&reg->b);
        break;

    case 0x21:
        sla(&reg->c);
        break;

    case 0x22:
        sla(&reg->d);
        break;

    case 0x23:
        sla(&reg->e);
        break;

    case 0x24:
        sla(&reg->h);
        break;

    case 0x25:
        sla(&reg->l);
        break;

    case 0x26:
        n = mem->read_byte(reg->hl);
        sla(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x27:
        sla(&reg->a);
        break;

    case 0x28:
        sra(&reg->b);
        break;

    case 0x29:
        sra(&reg->c);
        break;

    case 0x2A:
        sra(&reg->d);
        break;

    case 0x2B:
        sra(&reg->e);
        break;

    case 0x2C:
        sra(&reg->h);
        break;

    case 0x2D:
        sra(&reg->l);
        break;

    case 0x2E:
        n = mem->read_byte(reg->hl);
        sra(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x2F:
        sra(&reg->a);
        break;

    case 0x30:
        swap(&reg->b);
        break;

    case 0x31:
        swap(&reg->c);
        break;

    case 0x32:
        swap(&reg->d);
        break;

    case 0x33:
        swap(&reg->e);
        break;

    case 0x34:
        swap(&reg->h);
        break;

    case 0x35:
        swap(&reg->l);
        break;

    case 0x36:
        n = mem->read_byte(reg->hl);
        swap(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x37:
        swap(&reg->a);
        break;

    case 0x38:
        srl(&reg->b);
        break;

    case 0x39:
        srl(&reg->c);
        break;

    case 0x3A:
        srl(&reg->d);
        break;

    case 0x3B:
        srl(&reg->e);
        break;

    case 0x3C:
        srl(&reg->h);
        break;

    case 0x3D:
        srl(&reg->l);
        break;

    case 0x3E:
        n = mem->read_byte(reg->hl);
        srl(&n);
        mem->write_byte(reg->hl, n);
        break;

    case 0x3F:
        srl(&reg->a);
        break;

    case 0x40:
        bit(reg->b, 0);
        break;

    case 0x41:
        bit(reg->c, 0);
        break;

    case 0x42:
        bit(reg->d, 0);
        break;

    case 0x43:
        bit(reg->e, 0);
        break;

    case 0x44:
        bit(reg->h, 0);
        break;

    case 0x45:
        bit(reg->l, 0);
        break;

    case 0x46:
        bit(mem->read_byte(reg->hl), 0);
        break;

    case 0x47:
        bit(reg->a, 0);
        break;

    case 0x48:
        bit(reg->b, 1);
        break;

    case 0x49:
        bit(reg->c, 1);
        break;

    case 0x4A:
        bit(reg->d, 1);
        break;

    case 0x4B:
        bit(reg->e, 1);
        break;

    case 0x4C:
        bit(reg->h, 1);
        break;

    case 0x4D:
        bit(reg->l, 1);
        break;

    case 0x4E:
        bit(mem->read_byte(reg->hl), 1);
        break;

    case 0x4F:
        bit(reg->a, 1);
        break;

    case 0x50:
        bit(reg->b, 2);
        break;

    case 0x51:
        bit(reg->c, 2);
        break;

    case 0x52:
        bit(reg->d, 2);
        break;

    case 0x53:
        bit(reg->e, 2);
        break;

    case 0x54:
        bit(reg->h, 2);
        break;

    case 0x55:
        bit(reg->l, 2);
        break;

    case 0x56:
        bit(mem->read_byte(reg->hl), 2);
        break;

    case 0x57:
        bit(reg->a, 2);
        break;

    case 0x58:
        bit(reg->b, 3);
        break;

    case 0x59:
        bit(reg->c, 3);
        break;

    case 0x5A:
        bit(reg->d, 3);
        break;

    case 0x5B:
        bit(reg->e, 3);
        break;

    case 0x5C:
        bit(reg->h, 3);
        break;

    case 0x5D:
        bit(reg->l, 3);
        break;

    case 0x5E:
        bit(mem->read_byte(reg->hl), 3);
        break;

    case 0x5F:
        bit(reg->a, 3);
        break;

    case 0x60:
        bit(reg->b, 4);
        break;

    case 0x61:
        bit(reg->c, 4);
        break;

    case 0x62:
        bit(reg->d, 4);
        break;

    case 0x63:
        bit(reg->e, 4);
        break;

    case 0x64:
        bit(reg->h, 4);
        break;

    case 0x65:
        bit(reg->l, 4);
        break;

    case 0x66:
        bit(mem->read_byte(reg->hl), 4);
        break;

    case 0x67:
        bit(reg->a, 4);
        break;

    case 0x68:
        bit(reg->b, 5);
        break;

    case 0x69:
        bit(reg->c, 5);
        break;

    case 0x6A:
        bit(reg->d, 5);
        break;

    case 0x6B:
        bit(reg->e, 5);
        break;

    case 0x6C:
        bit(reg->h, 5);
        break;

    case 0x6D:
        bit(reg->l, 5);
        break;

    case 0x6E:
        bit(mem->read_byte(reg->hl), 5);
        break;

    case 0x6F:
        bit(reg->a, 5);
        break;

    case 0x70:
        bit(reg->b, 6);
        break;

    case 0x71:
        bit(reg->c, 6);
        break;

    case 0x72:
        bit(reg->d, 6);
        break;

    case 0x73:
        bit(reg->e, 6);
        break;

    case 0x74:
        bit(reg->h, 6);
        break;

    case 0x75:
        bit(reg->l, 6);
        break;

    case 0x76:
        bit(mem->read_byte(reg->hl), 6);
        break;

    case 0x77:
        bit(reg->a, 6);
        break;

    case 0x78:
        bit(reg->b, 7);
        break;

    case 0x79:
        bit(reg->c, 7);
        break;

    case 0x7A:
        bit(reg->d, 7);
        break;

    case 0x7B:
        bit(reg->e, 7);
        break;

    case 0x7C:
        bit(reg->h, 7);
        break;

    case 0x7D:
        bit(reg->l, 7);
        break;

    case 0x7E:
        bit(mem->read_byte(reg->hl), 7);
        break;

    case 0x7F:
        bit(reg->a, 7);
        break;

    case 0x80:
        utility::bitwise::reset_bit(&reg->b, 0);
        break;

    case 0x81:
        utility::bitwise::reset_bit(&reg->c, 0);
        break;

    case 0x82:
        utility::bitwise::reset_bit(&reg->d, 0);
        break;

    case 0x83:
        utility::bitwise::reset_bit(&reg->e, 0);
        break;

    case 0x84:
        utility::bitwise::reset_bit(&reg->h, 0);
        break;

    case 0x85:
        utility::bitwise::reset_bit(&reg->l, 0);
        break;

    case 0x86:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 0);
        mem->write_byte(reg->hl, n);
        break;

    case 0x87:
        utility::bitwise::reset_bit(&reg->a, 0);
        break;

    case 0x88:
        utility::bitwise::reset_bit(&reg->b, 1);
        break;

    case 0x89:
        utility::bitwise::reset_bit(&reg->c, 1);
        break;

    case 0x8A:
        utility::bitwise::reset_bit(&reg->d, 1);
        break;

    case 0x8B:
        utility::bitwise::reset_bit(&reg->e, 1);
        break;

    case 0x8C:
        utility::bitwise::reset_bit(&reg->h, 1);
        break;

    case 0x8D:
        utility::bitwise::reset_bit(&reg->l, 1);
        break;

    case 0x8E:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 1);
        mem->write_byte(reg->hl, n);
        break;

    case 0x8F:
        utility::bitwise::reset_bit(&reg->a, 1);
        break;

    case 0x90:
        utility::bitwise::reset_bit(&reg->b, 2);
        break;

    case 0x91:
        utility::bitwise::reset_bit(&reg->c, 2);
        break;

    case 0x92:
        utility::bitwise::reset_bit(&reg->d, 2);
        break;

    case 0x93:
        utility::bitwise::reset_bit(&reg->e, 2);
        break;

    case 0x94:
        utility::bitwise::reset_bit(&reg->h, 2);
        break;

    case 0x95:
        utility::bitwise::reset_bit(&reg->l, 2);
        break;

    case 0x96:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 2);
        mem->write_byte(reg->hl, n);
        break;

    case 0x97:
        utility::bitwise::reset_bit(&reg->a, 2);
        break;

    case 0x98:
        utility::bitwise::reset_bit(&reg->b, 3);
        break;

    case 0x99:
        utility::bitwise::reset_bit(&reg->c, 3);
        break;

    case 0x9A:
        utility::bitwise::reset_bit(&reg->d, 3);
        break;

    case 0x9B:
        utility::bitwise::reset_bit(&reg->e, 3);
        break;

    case 0x9C:
        utility::bitwise::reset_bit(&reg->h, 3);
        break;

    case 0x9D:
        utility::bitwise::reset_bit(&reg->l, 3);
        break;

    case 0x9E:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 3);
        mem->write_byte(reg->hl, n);
        break;

    case 0x9F:
        utility::bitwise::reset_bit(&reg->a, 3);
        break;

    case 0xA0:
        utility::bitwise::reset_bit(&reg->b, 4);
        break;

    case 0xA1:
        utility::bitwise::reset_bit(&reg->c, 4);
        break;

    case 0xA2:
        utility::bitwise::reset_bit(&reg->d, 4);
        break;

    case 0xA3:
        utility::bitwise::reset_bit(&reg->e, 4);
        break;

    case 0xA4:
        utility::bitwise::reset_bit(&reg->h, 4);
        break;

    case 0xA5:
        utility::bitwise::reset_bit(&reg->l, 4);
        break;

    case 0xA6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 4);
        mem->write_byte(reg->hl, n);
        break;

    case 0xA7:
        utility::bitwise::reset_bit(&reg->a, 4);
        break;

    case 0xA8:
        utility::bitwise::reset_bit(&reg->b, 5);
        break;

    case 0xA9:
        utility::bitwise::reset_bit(&reg->c, 5);
        break;

    case 0xAA:
        utility::bitwise::reset_bit(&reg->d, 5);
        break;

    case 0xAB:
        utility::bitwise::reset_bit(&reg->e, 5);
        break;

    case 0xAC:
        utility::bitwise::reset_bit(&reg->h, 5);
        break;

    case 0xAD:
        utility::bitwise::reset_bit(&reg->l, 5);
        break;

    case 0xAE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 5);
        mem->write_byte(reg->hl, n);
        break;

    case 0xAF:
        utility::bitwise::reset_bit(&reg->a, 5);
        break;

    case 0xB0:
        utility::bitwise::reset_bit(&reg->b, 6);
        break;

    case 0xB1:
        utility::bitwise::reset_bit(&reg->c, 6);
        break;

    case 0xB2:
        utility::bitwise::reset_bit(&reg->d, 6);
        break;

    case 0xB3:
        utility::bitwise::reset_bit(&reg->e, 6);
        break;

    case 0xB4:
        utility::bitwise::reset_bit(&reg->h, 6);
        break;

    case 0xB5:
        utility::bitwise::reset_bit(&reg->l, 6);
        break;

    case 0xB6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 6);
        mem->write_byte(reg->hl, n);
        break;

    case 0xB7:
        utility::bitwise::reset_bit(&reg->a, 6);
        break;

    case 0xB8:
        utility::bitwise::reset_bit(&reg->b, 7);
        break;

    case 0xB9:
        utility::bitwise::reset_bit(&reg->c, 7);
        break;

    case 0xBA:
        utility::bitwise::reset_bit(&reg->d, 7);
        break;

    case 0xBB:
        utility::bitwise::reset_bit(&reg->e, 7);
        break;

    case 0xBC:
        utility::bitwise::reset_bit(&reg->h, 7);
        break;

    case 0xBD:
        utility::bitwise::reset_bit(&reg->l, 7);
        break;

    case 0xBE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::reset_bit(&n, 7);
        mem->write_byte(reg->hl, n);
        break;

    case 0xBF:
        utility::bitwise::reset_bit(&reg->a, 7);
        break;

    case 0xC0:
        utility::bitwise::set_bit(&reg->b, 0);
        break;

    case 0xC1:
        utility::bitwise::set_bit(&reg->c, 0);
        break;

    case 0xC2:
        utility::bitwise::set_bit(&reg->d, 0);
        break;

    case 0xC3:
        utility::bitwise::set_bit(&reg->e, 0);
        break;

    case 0xC4:
        utility::bitwise::set_bit(&reg->h, 0);
        break;

    case 0xC5:
        utility::bitwise::set_bit(&reg->l, 0);
        break;

    case 0xC6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 0);
        mem->write_byte(reg->hl, n);
        break;

    case 0xC7:
        utility::bitwise::set_bit(&reg->a, 0);
        break;

    case 0xC8:
        utility::bitwise::set_bit(&reg->b, 1);
        break;

    case 0xC9:
        utility::bitwise::set_bit(&reg->c, 1);
        break;

    case 0xCA:
        utility::bitwise::set_bit(&reg->d, 1);
        break;

    case 0xCB:
        utility::bitwise::set_bit(&reg->e, 1);
        break;

    case 0xCC:
        utility::bitwise::set_bit(&reg->h, 1);
        break;

    case 0xCD:
        utility::bitwise::set_bit(&reg->l, 1);
        break;

    case 0xCE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 1);
        mem->write_byte(reg->hl, n);
        break;

    case 0xCF:
        utility::bitwise::set_bit(&reg->a, 1);
        break;

    case 0xD0:
        utility::bitwise::set_bit(&reg->b, 2);
        break;

    case 0xD1:
        utility::bitwise::set_bit(&reg->c, 2);
        break;

    case 0xD2:
        utility::bitwise::set_bit(&reg->d, 2);
        break;

    case 0xD3:
        utility::bitwise::set_bit(&reg->e, 2);
        break;

    case 0xD4:
        utility::bitwise::set_bit(&reg->h, 2);
        break;

    case 0xD5:
        utility::bitwise::set_bit(&reg->l, 2);
        break;

    case 0xD6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 2);
        mem->write_byte(reg->hl, n);
        break;

    case 0xD7:
        utility::bitwise::set_bit(&reg->a, 2);
        break;

    case 0xD8:
        utility::bitwise::set_bit(&reg->b, 3);
        break;

    case 0xD9:
        utility::bitwise::set_bit(&reg->c, 3);
        break;

    case 0xDA:
        utility::bitwise::set_bit(&reg->d, 3);
        break;

    case 0xDB:
        utility::bitwise::set_bit(&reg->e, 3);
        break;

    case 0xDC:
        utility::bitwise::set_bit(&reg->h, 3);
        break;

    case 0xDD:
        utility::bitwise::set_bit(&reg->l, 3);
        break;

    case 0xDE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 3);
        mem->write_byte(reg->hl, n);
        break;

    case 0xDF:
        utility::bitwise::set_bit(&reg->a, 3);
        break;

    case 0xE0:
        utility::bitwise::set_bit(&reg->b, 4);
        break;

    case 0xE1:
        utility::bitwise::set_bit(&reg->c, 4);
        break;

    case 0xE2:
        utility::bitwise::set_bit(&reg->d, 4);
        break;

    case 0xE3:
        utility::bitwise::set_bit(&reg->e, 4);
        break;

    case 0xE4:
        utility::bitwise::set_bit(&reg->h, 4);
        break;

    case 0xE5:
        utility::bitwise::set_bit(&reg->l, 4);
        break;

    case 0xE6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 4);
        mem->write_byte(reg->hl, n);
        break;

    case 0xE7:
        utility::bitwise::set_bit(&reg->a, 4);
        break;

    case 0xE8:
        utility::bitwise::set_bit(&reg->b, 5);
        break;

    case 0xE9:
        utility::bitwise::set_bit(&reg->c, 5);
        break;

    case 0xEA:
        utility::bitwise::set_bit(&reg->d, 5);
        break;

    case 0xEB:
        utility::bitwise::set_bit(&reg->e, 5);
        break;

    case 0xEC:
        utility::bitwise::set_bit(&reg->h, 5);
        break;

    case 0xED:
        utility::bitwise::set_bit(&reg->l, 5);
        break;

    case 0xEE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 5);
        mem->write_byte(reg->hl, n);
        break;

    case 0xEF:
        utility::bitwise::set_bit(&reg->a, 5);
        break;

    case 0xF0:
        utility::bitwise::set_bit(&reg->b, 6);
        break;

    case 0xF1:
        utility::bitwise::set_bit(&reg->c, 6);
        break;

    case 0xF2:
        utility::bitwise::set_bit(&reg->d, 6);
        break;

    case 0xF3:
        utility::bitwise::set_bit(&reg->e, 6);
        break;

    case 0xF4:
        utility::bitwise::set_bit(&reg->h, 6);
        break;

    case 0xF5:
        utility::bitwise::set_bit(&reg->l, 6);
        break;

    case 0xF6:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 6);
        mem->write_byte(reg->hl, n);
        break;

    case 0xF7:
        utility::bitwise::set_bit(&reg->a, 6);
        break;

    case 0xF8:
        utility::bitwise::set_bit(&reg->b, 7);
        break;

    case 0xF9:
        utility::bitwise::set_bit(&reg->c, 7);
        break;

    case 0xFA:
        utility::bitwise::set_bit(&reg->d, 7);
        break;

    case 0xFB:
        utility::bitwise::set_bit(&reg->e, 7);
        break;

    case 0xFC:
        utility::bitwise::set_bit(&reg->h, 7);
        break;

    case 0xFD:
        utility::bitwise::set_bit(&reg->l, 7);
        break;

    case 0xFE:
        n = mem->read_byte(reg->hl);
        utility::bitwise::set_bit(&n, 7);
        mem->write_byte(reg->hl, n);
        break;

    case 0xFF:
        utility::bitwise::set_bit(&reg->a, 7);
        break;
    }
    return cb_instructions[opcode].mcycles;
}

void Cpu::rlc(u8* r)
{
    u8 msb = utility::bitwise::get_bit(r, 7);
    reg->set_flag(Flag::CARRY, msb);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    *r = (*r << 1) | msb;
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::rrc(u8* r)
{
    u8 lsb = utility::bitwise::get_bit(r, 0);
    reg->set_flag(Flag::CARRY, lsb);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    *r = (*r >> 1) | (lsb << 7);
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::rl(u8* r)
{
    u8 tmp = reg->flag_is_set(Flag::CARRY);
    u8 msb = utility::bitwise::get_bit(r, 7);
    reg->set_flag(Flag::CARRY, msb);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    *r = (*r << 1) | tmp;
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::rr(u8* r)
{
    u8 tmp = reg->flag_is_set(Flag::CARRY);
    u8 lsb = utility::bitwise::get_bit(r, 0);
    reg->set_flag(Flag::CARRY, lsb);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::NEG, false);
    *r = (*r >> 1) | (tmp << 7);
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::sla(u8* r)
{
    u8 msb = utility::bitwise::get_bit(r, 7);
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::CARRY, msb);
    *r <<= 1;
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::sra(u8* r)
{
    u8 lsb = utility::bitwise::get_bit(r, 0);
    u8 msb = (*r & (1 << 7));
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::CARRY, lsb);
    *r >>= 1;
    *r |= msb;
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::swap(u8* r)
{
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::CARRY, false);
    u8 lo = (*r & 0x0F);
    u8 hi = (*r & 0xF0);
    *r = (lo << 4) | (hi >> 4);
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::srl(u8* r)
{
    u8 lsb = utility::bitwise::get_bit(r, 0);
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::HALF, false);
    reg->set_flag(Flag::CARRY, lsb);
    *r >>= 1;
    reg->set_flag(Flag::ZERO, !*r);
}

void Cpu::bit(u8 r, u8 pos)
{
    u8 copy = r;
    reg->set_flag(Flag::ZERO, !utility::bitwise::is_bit_set(&copy, pos));
    reg->set_flag(Flag::NEG, false);
    reg->set_flag(Flag::HALF, true);
}

};
