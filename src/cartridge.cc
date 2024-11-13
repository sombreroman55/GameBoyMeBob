#include "cartridge.hh"
#include <fstream>
#include <vector>

// TODO: Later, add support for serializing and deserialzing RAM data
//       to a file for save states on games that have ram

namespace gameboymebob {
Cartridge::Cartridge(std::string rom)
{
    std::ifstream file(rom, std::ios::binary);
    if (file) {
        std::streampos header_pos = 0x100;
        size_t header_size = 0x50;

        // read char data
        file.seekg(header_pos, std::ios::beg);
        std::vector<char> header_chars(header_size);
        file.read(header_chars.data(), header_size);

        // copy to u8 vector
        std::vector<u8> header_bytes(header_size);
        std::copy(header_chars.begin(), header_chars.end(), std::back_inserter(header_bytes));

        header.populate(header_bytes);

        rom.resize(header.rom_size);
        rom.clear();

        rom.insert(rom.begin(),
            std::istream_iterator<u8>(file),
            std::istream_iterator<u8>());

        if (header.ram_size) {
            ram.resize(header.ram_size);
        }
    }
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
