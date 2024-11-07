#pragma once

#include "utils/util.hh"
#include <string>

namespace gameboymebob {
class Cartridge {
public:
    std::vector<u8> rom_data;

    Cartridge(std::string rom);
    ~Cartridge();
};
};
