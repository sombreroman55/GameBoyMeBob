#include "gb.hh"

namespace gameboymebob {
GameBoy::GameBoy()
{
    mem = new Mmu();
    interrupts = new InterruptController(mem);
    serial = new SerialController(mem);
    cpu = new Cpu(mem, interrupts);
    ppu = new Ppu(mem, interrupts);
    timer = new Timer(mem, interrupts);
}

GameBoy::~GameBoy()
{
    delete cart;
    delete timer;
    delete cpu;
    delete serial;
    delete interrupts;
    delete mem;
}

void GameBoy::run(void)
{
    u32 cycles = cpu->step();
    ppu->step(cycles);
    timer->tick(cycles);
}

void GameBoy::insert_cartridge(Cartridge* c)
{
    cart = c;
    mem->map_cartridge(cart);
}

bool GameBoy::frame_ready(void)
{
    return ppu->frame_is_ready();
}

u8* GameBoy::get_frame(void)
{
    return ppu->get_viewport();
}
};
