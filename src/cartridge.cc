#include "cartridge.hh"
#include "mbc/mbc0.hh"
#include "mbc/mbc1.hh"
#include <fstream>
#include <vector>

// TODO: Later, add support for serializing and deserialzing RAM data
//       to a file for save states on games that have ram

namespace gameboymebob {
Cartridge::Cartridge(std::string rom_file)
{
    header.populate(rom_file);
    std::ifstream file(rom_file, std::ios::binary);
    if (file) {
        rom.resize(header.rom_size);
        rom.clear();

        file.unsetf(std::ios::skipws);

        rom.insert(rom.begin(),
            std::istream_iterator<u8>(file),
            std::istream_iterator<u8>());

        if (header.ram_size) {
            ram.resize(header.ram_size);
        }

        if (header.cart_type == "ROM ONLY") {
            bank_controller = new Mbc0(this);
        } else if (header.cart_type.find("MBC1") != std::string::npos) {
            bank_controller = new Mbc1(this);
        }
    }
    file.close();
}

Cartridge::~Cartridge() { }

u8 Cartridge::read_byte(u16 addr)
{
    return bank_controller->read_byte(addr);
}

void Cartridge::write_byte(u16 addr, u8 byte)
{
    bank_controller->write_byte(addr, byte);
}

};
