// Simple main file to start running Blargg files

#include "cpu.hh"
#include "mmu.hh"
#include "cartridge.hh"

int main (int argc, char *argv[]) {
    gameboymebob::Mmu* mem = new gameboymebob::Mmu();
    gameboymebob::Cartridge* cart = new gameboymebob::Cartridge("/home/droo/sources/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb");
    mem->load_cartridge(cart);
    gameboymebob::Cpu* cpu = new gameboymebob::Cpu(mem);
    while(true) {
        cpu->step();
    }
    return 0;
}
