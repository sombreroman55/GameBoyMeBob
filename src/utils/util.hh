#pragma once

#include "spdlog/spdlog.h"
#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

namespace utility {
namespace bitwise {
    inline void set_bit(u8* val, u8 pos)
    {
        *val |= (1 << pos);
    }

    inline void reset_bit(u8* val, u8 pos)
    {
        *val &= ~(1 << pos);
    }

    inline u8 get_bit(u8* val, u8 pos)
    {
        return (*val >> pos) & 0x1;
    }

    inline bool is_bit_set(u8* val, u8 pos)
    {
        return (*val & (1 << pos)) != 0;
    }
};
};
