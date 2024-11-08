#include "serial.hh"

namespace gameboymebob {
SerialController::SerialController(Mmu* mmu)
    : mem(mmu)
    , sb(&mem->memory[0xFF01])
    , sc(&mem->memory[0xFF02])
{
    *sb = 0x00;
    *sc = 0x7E;
}

SerialController::~SerialController() {}

void SerialController::load_transfer_byte(u8 byte)
{
    *sb = byte;
}

};
