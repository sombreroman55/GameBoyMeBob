#include "cpu.hh"
#include "instructions.hh"
#include "utils/util.hh"

namespace gameboymebob {
Cpu::Cpu(Mmu* mmu)
    : mem(mmu)
{
    reg = new Registers();
}

Cpu::Cpu()
{
    reg = new Registers();
}

Cpu::~Cpu()
{
    delete reg;
}

u32 Cpu::step(void)
{
    u32 cycles_consumed;
    u8 opcode = mem->read_byte(reg->pc);

    if (opcode == 0xCB) {
        // CB prefix
        u8 cb_opcode = mem->read_byte(reg->pc);
        cycles_consumed = execute_cb(cb_opcode);
    } else {
        cycles_consumed = execute(opcode);
    }

    return cycles_consumed;
}

// Normal opcodes
u32 Cpu::execute(u8 opcode)
{
    switch (opcode) {
    case 0x00: // nop
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
    case 0x10: // stop
        stopped = true;
        break;
    case 0x12:
        mem->write_byte(reg->de, reg->a);
        break;
    case 0x22:
        mem->write_byte(reg->hl++, reg->a);
        break;
    case 0x32:
        mem->write_byte(reg->hl--, reg->a);
        break;
    case 0x40:
        reg->b = reg->b;
        break;
    case 0x41:
        reg->b = reg->c;
        break;
    case 0x42:
        reg->b = reg->d;
        break;
    case 0x43:
        reg->b = reg->e;
        break;
    case 0x44:
        reg->b = reg->h;
        break;
    case 0x45:
        reg->b = reg->l;
        break;
    case 0x46:
        reg->b = mem->read_byte(reg->hl);
        break;
    case 0x47:
        reg->b = reg->a;
        break;
    case 0x48:
        reg->c = reg->b;
        break;
    case 0x49:
        reg->c = reg->c;
        break;
    case 0x4A:
        reg->c = reg->d;
        break;
    case 0x4B:
        reg->c = reg->e;
        break;
    case 0x4C:
        reg->c = reg->h;
        break;
    case 0x4D:
        reg->c = reg->l;
        break;
    case 0x4E:
        reg->c = mem->read_byte(reg->hl);
        break;
    case 0x4F:
        reg->c = reg->a;
        break;
    case 0x50:
        reg->d = reg->b;
        break;
    case 0x51:
        reg->d = reg->c;
        break;
    case 0x52:
        reg->d = reg->d;
        break;
    case 0x53:
        reg->d = reg->e;
        break;
    case 0x54:
        reg->d = reg->h;
        break;
    case 0x55:
        reg->d = reg->l;
        break;
    case 0x56:
        reg->d = mem->read_byte(reg->hl);
        break;
    case 0x57:
        reg->d = reg->a;
        break;
    case 0x58:
        reg->e = reg->b;
        break;
    case 0x59:
        reg->e = reg->c;
        break;
    case 0x5A:
        reg->e = reg->d;
        break;
    case 0x5B:
        reg->e = reg->e;
        break;
    case 0x5C:
        reg->e = reg->h;
        break;
    case 0x5D:
        reg->e = reg->l;
        break;
    case 0x5E:
        reg->e = mem->read_byte(reg->hl);
        break;
    case 0x5F:
        reg->e = reg->a;
        break;
    case 0x60:
        reg->h = reg->b;
        break;
    case 0x61:
        reg->h = reg->c;
        break;
    case 0x62:
        reg->h = reg->d;
        break;
    case 0x63:
        reg->h = reg->e;
        break;
    case 0x64:
        reg->h = reg->h;
        break;
    case 0x65:
        reg->h = reg->l;
        break;
    case 0x66:
        reg->h = mem->read_byte(reg->hl);
        break;
    case 0x67:
        reg->h = reg->a;
        break;
    case 0x68:
        reg->l = reg->b;
        break;
    case 0x69:
        reg->l = reg->c;
        break;
    case 0x6A:
        reg->l = reg->d;
        break;
    case 0x6B:
        reg->l = reg->e;
        break;
    case 0x6C:
        reg->l = reg->h;
        break;
    case 0x6D:
        reg->l = reg->l;
        break;
    case 0x6E:
        reg->l = mem->read_byte(reg->hl);
        break;
    case 0x6F:
        reg->l = reg->a;
        break;
    case 0x70:
        mem->write_byte(reg->hl, reg->b);
        break;
    case 0x71:
        mem->write_byte(reg->hl, reg->c);
        break;
    case 0x72:
        mem->write_byte(reg->hl, reg->d);
        break;
    case 0x73:
        mem->write_byte(reg->hl, reg->e);
        break;
    case 0x74:
        mem->write_byte(reg->hl, reg->h);
        break;
    case 0x75:
        mem->write_byte(reg->hl, reg->l);
        break;
    case 0x76:
        // TODO: Implement the halt bug logic here
        halted = true;
        break;
    case 0x77:
        mem->write_byte(reg->hl, reg->a);
        break;
    case 0x78:
        reg->a = reg->b;
        break;
    case 0x79:
        reg->a = reg->c;
        break;
    case 0x7A:
        reg->a = reg->d;
        break;
    case 0x7B:
        reg->a = reg->e;
        break;
    case 0x7C:
        reg->a = reg->h;
        break;
    case 0x7D:
        reg->a = reg->l;
        break;
    case 0x7E:
        reg->a = mem->read_byte(reg->hl);
        break;
    case 0x7F:
        reg->a = reg->a;
        break;
    default:
        spdlog::info("Opcode 0x{:02X} not yet implemented", opcode);
        break;
    }
    return instructions[opcode].mcycles;
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
    reg->set_flag(Flag::HALF, !(*r & 0x0f));
    *r -= 1;
    reg->set_flag(Flag::ZERO, !*r);
    reg->set_flag(Flag::NEG, true);
}



// CB opcodes
u32 Cpu::execute_cb(u8 opcode)
{
    u8 n = 0;
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
        sra(&reg->b);
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
    spdlog::trace("Executed CB opcode {:02X}", opcode);
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
