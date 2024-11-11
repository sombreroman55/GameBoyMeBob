#include "serial.hh"

namespace gameboymebob {
SerialController::SerialController(Mmu* mmu)
    : mem(mmu)
    , sb(&mem->memory[IoRegisters::sb])
    , sc(&mem->memory[IoRegisters::sc])
{
    *sb = 0x00;
    *sc = 0x7E;
    mem->map_serial(this);
}

SerialController::~SerialController() {}

void SerialController::load_transfer_byte(u8 byte)
{
    *sb = byte;
}

};
