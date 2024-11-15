// Simple main file to start running Blargg files

#include "cartridge.hh"
#include "gb.hh"
#include <string>

int main(int argc, char* argv[])
{
    std::string rom_file(argv[1]);

    gameboymebob::GameBoy* gb = new gameboymebob::GameBoy();
    gameboymebob::Cartridge* cart = new gameboymebob::Cartridge(rom_file);
    gb->insert_cartridge(cart);
    gb->run();
    delete gb;

    return 0;
}
