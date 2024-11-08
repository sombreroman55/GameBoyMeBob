#include "cpu.hh"
#include "instructions.hh"

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

u32 Cpu::execute(u8 opcode)
{
    switch (opcode) {
    case 0x00: // nop
        break;
    case 0x02:
        mem->write_byte(reg->bc, reg->a);
        break;
    case 0x10: // nop
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
    }
    return instructions[opcode].mcycles;
}

u32 Cpu::execute_cb(u8 opcode)
{
    switch (opcode) {
    }
    return cb_instructions[opcode].mcycles;
}

};
