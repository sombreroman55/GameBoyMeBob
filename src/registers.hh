#pragma once

#include "utils/util.hh"

namespace gameboymebob {
enum class Flag {
    ZERO = 7,
    NEG = 6,
    HALF = 5,
    CARRY = 4
};

class Registers {
public:
    union {
        struct {
            u8 f;
            u8 a;
        };
        u16 af;
    };

    union {
        struct {
            u8 c;
            u8 b;
        };
        u16 bc;
    };

    union {
        struct {
            u8 e;
            u8 d;
        };
        u16 de;
    };

    union {
        struct {
            u8 l;
            u8 h;
        };
        u16 hl;
    };

    u16 sp;
    u16 pc;

    Registers();
    ~Registers() {};

    void print_registers(void);
    void print_flags(void);

    bool flag_is_set(Flag flag);
    void set_flag(Flag flag, bool set);
};
};
