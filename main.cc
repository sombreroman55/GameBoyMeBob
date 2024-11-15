// Simple main file to start running Blargg files

#include "cartridge.hh"
#include "cpu.hh"
#include "interrupts.hh"
#include "mmu.hh"
#include "serial.hh"
#include "timer.hh"
#include <string>

int main(int argc, char* argv[])
{
    std::string rom_file(argv[1]);

    gameboymebob::Mmu* mem = new gameboymebob::Mmu();
    gameboymebob::Cartridge* cart = new gameboymebob::Cartridge(rom_file);
    mem->map_cartridge(cart);
    gameboymebob::InterruptController* ic = new gameboymebob::InterruptController(mem);
    gameboymebob::SerialController* sc = new gameboymebob::SerialController(mem);
    gameboymebob::Cpu* cpu = new gameboymebob::Cpu(mem, ic);
    gameboymebob::Timer* timer = new gameboymebob::Timer(mem, ic);
    while (true) {
        u32 cycles = cpu->step();
        timer->tick(cycles);
    }

    delete timer;
    delete cpu;
    delete sc;
    delete ic;
    delete cart;
    delete mem;

    return 0;
}
