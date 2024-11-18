#pragma once

#include "interrupts.hh"
#include "utils/util.hh"
#include <array>
#include <vector>

namespace gameboymebob {
enum class PpuMode {
    HBlank,
    VBlank,
    OamSearch,
    PixelTransfer
};

enum PpuConstants {
    HEIGHT = 144,
    WIDTH = 160,
    CLOCKS_PER_LINE = 114,
    CLOCKS_PER_FRAME = 17556,
    VBLANK_CLOCKS = 144 * CLOCKS_PER_LINE,
};

enum StatBits {
    mode_lo = 0,
    mode_hi = 1,
    ly_eq_lyc = 2,
    mode0_int = 3,
    mode1_int = 4,
    mode2_int = 5,
    lyc_int = 6,
};

enum LcdcBits {
    bg_win_enable = 0,
    obj_enable = 1,
    obj_size = 2,
    bg_tilemap = 3,
    bg_win_tiledata = 4,
    win_enable = 5,
    win_tilemap = 6,
    ppu_enable = 7,
};

enum ObjFlags {
    cgb_pal_lo = 0,
    cgb_pal_hi = 2,
    bank = 3,
    dmg_pal = 4,
    x_flip = 5,
    y_flip = 6,
    priority = 7,
};

class Ppu {
private:
    std::array<u8, 0x2000> vram = {};
    std::array<u8, 0xA0> oam_ram = {};

    std::array<std::array<u8, 256>, 256> bg_map = {};
    std::array<std::array<u8, 256>, 256> win_map = {};
    std::array<std::array<u8, 160>, 144> viewport = {};

    // Palettes
    std::array<u8, 4> bg_palette = {};
    std::array<u8, 4> obj0_palette = {};
    std::array<u8, 4> obj1_palette = {};

    // Stores the index of the visible sprite in OAM
    std::array<int,10> visible_sprites;
    int num_sprites = 0;

    Mmu* mem;
    InterruptController* interrupts;

    u8* lcdc;
    u8* stat;
    u8* scy;
    u8* scx;
    u8* ly;
    u8* lyc;
    u8* dma;
    u8* bgp;
    u8* obp0;
    u8* obp1;
    u8* wy;
    u8* wx;

    u32 cycle_count = 0;
    PpuMode mode;
    bool frame_ready = false;
    bool vram_dirty = false;

    void hblank(void);
    void vblank(void);
    void oam_search(void);
    void pixel_transfer(void);
    void refresh_maps(void);
    PpuMode calculate_mode(void);

public:
    Ppu(Mmu* mmu, InterruptController* ic);
    ~Ppu();

    void step(u32 cycles);

    u8 read_vram_byte(u16 addr);
    void write_vram_byte(u16 addr, u8 byte);
    u8 read_oam_ram_byte(u16 addr);
    void write_oam_ram_byte(u16 addr, u8 byte);

    // Update palettes
    void write_bg_palette(u8 byte);
    void write_obj0_palette(u8 byte);
    void write_obj1_palette(u8 byte);

    bool frame_is_ready(void);

    u8* get_viewport(void);
    u8* get_vram(void);
    u8* get_oam_ram(void);
    u8* get_bg_map(void);
    u8* get_win_map(void);
};
}
