#include "cartridge.hh"
#include "mbc/mbc0.hh"
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

    // parse the header
    parse_header();
}

Cartridge::~Cartridge() { }

void Cartridge::parse_header(void)
{
    // header.cgb_flag             = rom_data[0x143];
    header.new_licensee_code[0] = rom_data[0x144];
    header.new_licensee_code[1] = rom_data[0x145];
    header.sgb_flag = rom_data[0x146];
    header.cart_type = rom_data[0x147];
    header.rom_size = rom_data[0x148];
    header.ram_size = rom_data[0x149];
    header.destination = rom_data[0x14A];
    header.old_licensee_code = rom_data[0x14B];
    header.version_number = rom_data[0x14C];
    header.checksum = rom_data[0x14D];

    header.global_checksum = ((rom_data[0x14E] << 8) | rom_data[0x14F]);

    // TODO: Add the other bank controllers here
    if (header.cart_type == 0x00) {
        bank_controller = new Mbc0(this);
    }

    header.print();
}

u8 Cartridge::read_byte(u16 addr)
{
    return bank_controller->read_byte(addr);
}

void Cartridge::write_byte(u16 addr, u8 byte)
{
    bank_controller->write_byte(addr, byte);
}

};
