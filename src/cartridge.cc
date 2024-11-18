#include "cartridge.hh"
#include "mbc/mbc0.hh"
#include "mbc/mbc1.hh"
#include "utils/util.hh"
#include <fstream>
#include <vector>

// TODO: Later, add support for serializing and deserialzing RAM data
//       to a file for save states on games that have ram

namespace gameboymebob {
Cartridge::Cartridge(std::string rom_file)
{
    if (utility::file_utils::is_gb_extension(rom_file)) {
        std::ifstream file(rom_file, std::ios::binary);
        if (file) {
            std::streampos header_pos = 0x100;
            size_t header_size = 0x50;

            file.unsetf(std::ios::skipws);

            file.seekg(header_pos, std::ios::beg);
            std::vector<char> header_chars(header_size);
            file.read(header_chars.data(), header_size);

            std::vector<u8> header_bytes(header_size);
            for (int i = 0; i < 0x50; i++) {
                header_bytes[i] = static_cast<u8>(header_chars[i]);
            }

            header.parse(header_bytes);

            rom.resize(header.rom_size);
            rom.clear();

            file.seekg(0, std::ios::beg);
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
    } else if (utility::file_utils::is_zip_extension(rom_file)) {
        std::vector<u8> data = utility::file_utils::unzip_to_memory(rom_file);
        size_t header_size = 0x50;
        std::vector<u8> header_bytes(header_size);
        for (int i = 0; i < 0x50; i++) {
            header_bytes[i] = data[0x100 + i];
        }

        header.parse(header_bytes);

        rom.resize(header.rom_size);
        rom.clear();

        for (int i = 0; i < header.rom_size; i++) {
            rom[i] = data[i];
        }

        if (header.ram_size) {
            ram.resize(header.ram_size);
        }

        if (header.cart_type == "ROM ONLY") {
            bank_controller = new Mbc0(this);
        } else if (header.cart_type.find("MBC1") != std::string::npos) {
            bank_controller = new Mbc1(this);
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
