#include "cartridge.hh"
#include <fstream>

namespace gameboymebob {
Cartridge::Cartridge(std::string rom)
{
    // open the file:
    std::ifstream file(rom, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    rom_data.resize(fileSize);
    rom_data.clear();

    // read the data:
    rom_data.insert(rom_data.begin(),
        std::istream_iterator<u8>(file),
        std::istream_iterator<u8>());
}

Cartridge::~Cartridge() { }
}
