#pragma once

#include "cartridge.hh"
#include "utils/util.hh"
#include <array>
#include <utility>

namespace gameboymebob {

// forward declarations for compiler
class Joypad;
class Ppu;
class SerialController;
class Timer;

// clang-format off
enum IoRegisters : u16 {
    // Joypad
    joyp  = 0xFF00,

    // Serial
    sb    = 0xFF01,
    sc    = 0xFF02,

    // Timer
    div   = 0xFF04,
    tima  = 0xFF05,
    tma   = 0xFF06,
    tac   = 0xFF07,

    // Interrupts
    _if   = 0xFF0F,
    _ie   = 0xFFFF,

    // APU
    nr10  = 0xFF10,
    nr11  = 0xFF11,
    nr12  = 0xFF12,
    nr13  = 0xFF13,
    nr14  = 0xFF14,
    nr21  = 0xFF16,
    nr22  = 0xFF17,
    nr23  = 0xFF18,
    nr24  = 0xFF19,
    nr30  = 0xFF1A,
    nr31  = 0xFF1B,
    nr32  = 0xFF1C,
    nr33  = 0xFF1D,
    nr34  = 0xFF1E,
    nr41  = 0xFF20,
    nr42  = 0xFF21,
    nr43  = 0xFF22,
    nr44  = 0xFF23,
    nr50  = 0xFF24,
    nr51  = 0xFF25,
    nr52  = 0xFF26,

    // PPU
    lcdc  = 0xFF40,
    stat  = 0xFF41,
    scy   = 0xFF42,
    scx   = 0xFF43,
    ly    = 0xFF44,
    lyc   = 0xFF45,
    dma   = 0xFF46,
    bgp   = 0xFF47,  // DMG only
    obp0  = 0xFF48,  // DMG only
    obp1  = 0xFF49,  // DMG only
    wy    = 0xFF4A,
    wx    = 0xFF4B,

    // CGB registers (don't really plan on using these at the moment)
    key1  = 0xFF4D,  // Prepare speed switch
    vbk   = 0xFF4F,  // VRAM bank
    hdma1 = 0xFF51,  // VRAM DMA source high
    hdma2 = 0xFF52,  // VRAM DMA source low
    hdma3 = 0xFF53,  // VRAM DMA dest high
    hdma4 = 0xFF54,  // VRAM DMA dest low
    hdma5 = 0xFF55,  // VRAM DMA length/mode/start
    rp    = 0xFF56,  // Infrared communcations port
    bgpi  = 0xFF68,  // Background color palette specification / Background palette index
    bgpd  = 0xFF69,  // Background color palette data / Background palette data
    obpi  = 0xFF6A,  // OBJ color palette specification / OBJ palette index
    obpd  = 0xFF6B,  // OBJ color palette data / OBJ palette data
    opri  = 0xFF6C,  // Object priority mode
    svbk  = 0xFF70,  // WRAM bank
    pcm12 = 0xFF76,  // Audio digital outputs 1 & 2
    pcm34 = 0xFF77,  // Audio digital outputs 3 & 4
};
// clang-format on

// clang-format off
namespace memory_map {
    constexpr MemRange rom_bank0  = { 0x0000, 0x3FFF };
    constexpr MemRange rom_bankN  = { 0x4000, 0x7FFF };
    constexpr MemRange video_ram  = { 0x8000, 0x9FFF };
    constexpr MemRange ext_ram    = { 0xA000, 0xBFFF };
    constexpr MemRange work_ram0  = { 0xC000, 0xCFFF };
    constexpr MemRange work_ramN  = { 0xD000, 0xDFFF };
    constexpr MemRange echo_ram   = { 0xE000, 0xFDFF };
    constexpr MemRange oam_ram    = { 0xFE00, 0xFE9F };
    constexpr MemRange prohibited = { 0xFEA0, 0xFEFF };
    constexpr MemRange io_region  = { 0xFF00, 0xFF7F };
    constexpr MemRange wave_ram   = { 0xFF30, 0xFF3F };
    constexpr MemRange high_ram   = { 0xFF80, 0xFFFE };
}
// clang-format on

class Mmu {
private:
    u8 read_io_byte(u16 addr);
    void write_io_byte(u16 addr, u8 byte);

public:
    std::array<u8, 0x10000> memory = {};

    // Memory mapped components
    Cartridge* cart = nullptr;
    Joypad* joypad = nullptr;
    Ppu* ppu = nullptr;
    SerialController* serial = nullptr;
    Timer* timer = nullptr;

    Mmu();
    ~Mmu();

    // Device mapping functions
    void map_cartridge(Cartridge* c);
    void map_joyad(Joypad* jp);
    void map_ppu(Ppu* p);
    void map_serial(SerialController* ser);
    void map_timer(Timer* tm);

    u8 read_byte(u16 addr);
    u16 read_word(u16 addr);

    void write_byte(u16 addr, u8 byte);
    void write_word(u16 addr, u16 word);

    void push_stack(u16* stack, u16 word);
    u16 pop_stack(u16* stack);

    // TODO: Later, add ppu utility reads for tiles
};
};
