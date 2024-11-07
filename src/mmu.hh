#pragma once

#include "utils/util.hh"
#include <array>

namespace gameboymebob {
    class Mmu {
        private:
            std::array<u8, 0x10000> memory;

        public:
            Mmu();
            ~Mmu();

            u8 read_byte(u16 addr);
            u16 read_word(u16 addr);

            void write_byte(u16 addr, u8 byte);
            void write_word(u16 addr, u16 word);
    };
};
