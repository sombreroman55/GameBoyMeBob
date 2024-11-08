#pragma once

#include "mmu.hh"
#include "utils/util.hh"

namespace gameboymebob {
class SerialController {
    public:
        SerialController(Mmu* mmu);
        ~SerialController();

        void load_transfer_byte(u8 byte);

        Mmu* mem;
        u8* sb;
        u8* sc;
};
};
