#include "ppu.hh"
#include "utils/util.hh"

namespace gameboymebob {
Ppu::Ppu(Mmu* mmu, InterruptController* ic)
    : mem(mmu)
    , interrupts(ic)
    , lcdc(&mem->memory[IoRegisters::lcdc])
    , stat(&mem->memory[IoRegisters::stat])
    , scy(&mem->memory[IoRegisters::scy])
    , scx(&mem->memory[IoRegisters::scx])
    , ly(&mem->memory[IoRegisters::ly])
    , lyc(&mem->memory[IoRegisters::lyc])
    , dma(&mem->memory[IoRegisters::dma])
    , bgp(&mem->memory[IoRegisters::bgp])
    , obp0(&mem->memory[IoRegisters::obp0])
    , obp1(&mem->memory[IoRegisters::obp1])
    , wy(&mem->memory[IoRegisters::wy])
    , wx(&mem->memory[IoRegisters::wx])
    , mode(PpuMode::VBlank)
{
    *lcdc = 0x91;
    *stat = 0x85;
    *scy = 0x00;
    *scx = 0x00;
    *ly = 0x00;
    *lyc = 0x00;
    *dma = 0xFF;
    *bgp = 0xFC;
    *obp0 = 0x00;
    *obp1 = 0x00;
    *wy = 0x00;
    *wx = 0x00;
    write_bg_palette(*bgp);
    write_obj0_palette(*obp0);
    write_obj1_palette(*obp1);
    mem->map_ppu(this);
}

Ppu::~Ppu() { }

PpuMode Ppu::calculate_mode(void)
{
    enum {
        oam_search_clocks = 20,
        pxtx_clocks = 43,
        hblank_start = oam_search_clocks + pxtx_clocks
    };

    bool enabled = utility::bitwise::is_bit_set(lcdc, LcdcBits::ppu_enable);
    if (!enabled) {
        if (cycle_count >= PpuConstants::VBLANK_CLOCKS) {
            // Allow the renderer to draw a frame anyway
            // frame_ready = true;
        }
        return PpuMode::HBlank;
    }

    if (cycle_count >= PpuConstants::VBLANK_CLOCKS) {
        return PpuMode::VBlank;
    }

    int scan_clock = cycle_count % PpuConstants::CLOCKS_PER_LINE;

    if (0 <= scan_clock && scan_clock < oam_search_clocks) {
        return PpuMode::OamSearch;
    }
    if (oam_search_clocks <= scan_clock && scan_clock < hblank_start) {
        return PpuMode::PixelTransfer;
    }
    return PpuMode::HBlank;
}

void Ppu::hblank(void)
{
    bool enabled = utility::bitwise::is_bit_set(lcdc, LcdcBits::ppu_enable);
    if (enabled && utility::bitwise::is_bit_set(stat, StatBits::mode0_int)) {
        interrupts->set_interrupt(Interrupt::LCDSTAT);
    }
}

void Ppu::vblank(void)
{
    frame_ready = true;
    interrupts->set_interrupt(Interrupt::VBLANK);
    if (utility::bitwise::is_bit_set(stat, StatBits::mode1_int)) {
        interrupts->set_interrupt(Interrupt::LCDSTAT);
    }
    refresh_maps();
}

void Ppu::oam_search(void)
{
    num_sprites = 0;

    if (utility::bitwise::is_bit_set(stat, StatBits::mode2_int)) {
        interrupts->set_interrupt(Interrupt::LCDSTAT);
    }

    if (!utility::bitwise::is_bit_set(lcdc, LcdcBits::obj_enable)) {
        return;
    }

    bool double_size = utility::bitwise::is_bit_set(lcdc, LcdcBits::obj_size);
    int h = double_size ? 16 : 8;

    for (int i = 0; i < 40 && num_sprites < 10; i += (1 + double_size)) {
        u8 objy = oam_ram[(i*4) + 0];
        u8 objx = oam_ram[(i*4) + 1];

        if (objx != 0 && objy <= *ly + 16 && *ly + 16 < objy + h) {
            visible_sprites[num_sprites++] = (i*4);
        }
    }
}

void Ppu::pixel_transfer(void)
{
    typedef struct {
        u8 palette_idx;
        u8 palette_source;
    } PixelData;

    PixelData pixels[160];

    // Background
    bool bg_enabled = utility::bitwise::is_bit_set(lcdc, LcdcBits::bg_win_enable);
    int y = (*scy + *ly) % 256;
    for (int p = 0; p < 160; p++) {
        int x = (*scx + p) % 256;
        pixels[p].palette_idx = (bg_enabled ? bg_map[y][x] : 0);
        pixels[p].palette_source = 0;
    }

    // Window (if applicable to this scanline)
    bool win_enabled = bg_enabled && utility::bitwise::is_bit_set(lcdc, LcdcBits::win_enable);
    if (win_enabled && *ly >= *wy) {
        int y = *ly - *wy;
        for (int p = *wx; p < 160; p++) {
            int x = (p - *wx);
            pixels[p].palette_idx = win_map[y][x];
            pixels[p].palette_source = 1;
        }
    }

    // Sprites (only do this work if there are sprites to draw)
    if (num_sprites) {
        for (int p = 0; p < 160; p++) {
            // Check if pixel is within sprite
            for (int i = 0; i < num_sprites; i++) {
                u8 objy = oam_ram[visible_sprites[i] + 0];
                u8 objx = oam_ram[visible_sprites[i] + 1];
                u8 tile = oam_ram[visible_sprites[i] + 2];
                u8 flag = oam_ram[visible_sprites[i] + 3];

                if (objx == p) {
                    // Overlay the 8 pixels needed onto the pixel buffer
                    int height = utility::bitwise::is_bit_set(lcdc, LcdcBits::obj_size) ? 16 : 8;
                    std::vector<u16> tile_rows(height);
                    int tile_addr = tile * 16;
                    for (int i = 0; i < height; i++) {
                        tile_rows[i] = vram[tile_addr + 1] << 8 | vram[tile_addr];
                        tile_addr += 2;
                    }

                    if (utility::bitwise::is_bit_set(&flag, ObjFlags::y_flip)) {
                        std::reverse(tile_rows.begin(), tile_rows.end());
                    }

                    int curr_row = (*ly + 16) - objy;
                    u16 pixel_row = tile_rows[curr_row];

                    std::vector<u8> pixel_data(8);
                    u8 hi_byte = ((pixel_row >> 8) & 0xFF);
                    u8 lo_byte = (pixel_row & 0xFF);
                    for (int i = 0; i < 8; i++) {
                        u8 hi_bit = utility::bitwise::get_bit(&hi_byte, 7 - i);
                        u8 lo_bit = utility::bitwise::get_bit(&lo_byte, 7 - i);
                        pixel_data[i] = (hi_bit << 1) | lo_bit;
                    }

                    if (utility::bitwise::is_bit_set(&flag, ObjFlags::x_flip)) {
                        std::reverse(pixel_data.begin(), pixel_data.end());
                    }

                    u16 obj_palette = (utility::bitwise::is_bit_set(&flag, ObjFlags::dmg_pal) ? 3 : 2);

                    int x = objx - 8;
                    for (int j = 0; j < 8; j++) {
                        int pos = x + j;
                        if (pos < 0 || pos >= 160)
                            continue;
                        // Skip transparent pixels
                        if (pixel_data[j] == 0x00)
                            continue;
                        if (pixels[pos].palette_source < 2) {
                            if (utility::bitwise::is_bit_set(&flag, ObjFlags::priority)) {
                                if (pixels[pos].palette_idx == 0x00) {
                                    pixels[pos].palette_idx = pixel_data[j];
                                    pixels[pos].palette_source = obj_palette;
                                }
                            } else {
                                pixels[pos].palette_idx = pixel_data[j];
                                pixels[pos].palette_source = obj_palette;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int p = 0; p < 160; p++) {
        int color_idx = 0;
        if (pixels[p].palette_source == 0 || pixels[p].palette_source == 1) {
            color_idx = bg_palette[pixels[p].palette_idx];
        } else if (pixels[p].palette_source == 2) {
            color_idx = obj0_palette[pixels[p].palette_idx];
        } else if (pixels[p].palette_source == 3) {
            color_idx = obj1_palette[pixels[p].palette_idx];
        }
        viewport[*ly][p] = color_idx;
    }
}

void Ppu::refresh_maps(void)
{
    if (!vram_dirty) {
        return;
    }

    bool use_8000_method = utility::bitwise::is_bit_set(lcdc, LcdcBits::bg_win_tiledata);
    u16 bg_win_vram_base_addr = use_8000_method ? 0 : 0x1000;

    // Background map
    u16 bg_tilemap_base_addr = 0x1800;
    if (utility::bitwise::is_bit_set(lcdc, LcdcBits::bg_tilemap)) {
        bg_tilemap_base_addr += 0x400;
    }

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            int tilemap_num = i * 32 + j;
            int tilemap_idx = tilemap_num + bg_tilemap_base_addr;
            u8 tile_idx = vram[tilemap_idx];
            int tile_addr;
            if (use_8000_method) {
                tile_addr = tile_idx * 16 + bg_win_vram_base_addr;
            } else {
                tile_addr = static_cast<i8>(tile_idx) * 16 + bg_win_vram_base_addr;
            }
            for (int k = 0; k < 8; k++) {
                u8 tile_row_hi = vram[tile_addr + 1];
                u8 tile_row_lo = vram[tile_addr];
                for (int l = 0; l < 8; l++) {
                    u8 hi_bit = utility::bitwise::get_bit(&tile_row_hi, 7 - l);
                    u8 lo_bit = utility::bitwise::get_bit(&tile_row_lo, 7 - l);
                    bg_map[i * 8 + k][j * 8 + l] = (hi_bit << 1) | lo_bit;
                }
                tile_addr += 2;
            }
        }
    }

    // Window map
    u16 win_tilemap_base_addr = 0x1800;
    if (utility::bitwise::is_bit_set(lcdc, LcdcBits::win_tilemap)) {
        win_tilemap_base_addr += 0x400;
    }

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            int tilemap_num = i * 32 + j;
            int tilemap_idx = tilemap_num + win_tilemap_base_addr;
            u8 tile_idx = vram[tilemap_idx];
            int tile_addr;
            if (use_8000_method) {
                tile_addr = tile_idx * 16 + bg_win_vram_base_addr;
            } else {
                tile_addr = static_cast<i8>(tile_idx) * 16 + bg_win_vram_base_addr;
            }
            for (int k = 0; k < 8; k++) {
                u8 tile_row_hi = vram[tile_addr + 1];
                u8 tile_row_lo = vram[tile_addr];
                for (int l = 0; l < 8; l++) {
                    u8 hi_bit = utility::bitwise::get_bit(&tile_row_hi, 7 - l);
                    u8 lo_bit = utility::bitwise::get_bit(&tile_row_lo, 7 - l);
                    win_map[i * 8 + k][j * 8 + l] = (hi_bit << 1) | lo_bit;
                }
                tile_addr += 2;
            }
        }
    }

    vram_dirty = false;
}

void Ppu::step(u32 cycles)
{
    cycle_count += cycles;
    cycle_count %= PpuConstants::CLOCKS_PER_FRAME;
    PpuMode last_mode = static_cast<PpuMode>(*stat & 0x03);
    mode = calculate_mode();

    u8 last_ly = *ly;
    *ly = cycle_count / PpuConstants::CLOCKS_PER_LINE;
    if (*ly != last_ly) {
        utility::bitwise::assign_bit(stat, StatBits::ly_eq_lyc, (*ly == *lyc));
        if (utility::bitwise::is_bit_set(stat, StatBits::ly_eq_lyc) && utility::bitwise::is_bit_set(stat, StatBits::lyc_int)) {
            interrupts->set_interrupt(Interrupt::LCDSTAT);
        }
    }

    // PPU state machine
    if (mode != last_mode) {
        switch (last_mode) {
        case PpuMode::HBlank:
            if (mode == PpuMode::VBlank) {
                vblank();
            } else if (mode == PpuMode::OamSearch) {
                oam_search();
            }
            break;
        case PpuMode::VBlank:
            oam_search();
            break;
        case PpuMode::OamSearch:
            pixel_transfer();
            break;
        case PpuMode::PixelTransfer:
            hblank();
            break;
        }

        // Store new mode back to stat
        u8 copy = *stat;
        copy &= ~(0x03);
        copy |= static_cast<u8>(mode);
        *stat = copy;
    }
}

u8 Ppu::read_vram_byte(u16 addr)
{
    return vram[addr];
}

void Ppu::write_vram_byte(u16 addr, u8 byte)
{
    vram[addr] = byte;
    vram_dirty = true;
}

u8 Ppu::read_oam_ram_byte(u16 addr)
{
    return oam_ram[addr];
}

void Ppu::write_oam_ram_byte(u16 addr, u8 byte)
{
    oam_ram[addr] = byte;
}

void Ppu::write_bg_palette(u8 byte)
{
    for (int i = 0; i < 4; i++) {
        bg_palette[i] = byte & 0x03;
        byte >>= 2;
    }
}

void Ppu::write_obj0_palette(u8 byte)
{
    spdlog::info("Writing {} to obp0", byte);
    for (int i = 0; i < 4; i++) {
        obj0_palette[i] = byte & 0x03;
        byte >>= 2;
    }
}

void Ppu::write_obj1_palette(u8 byte)
{
    spdlog::info("Writing {} to obp1", byte);
    for (int i = 0; i < 4; i++) {
        obj1_palette[i] = byte & 0x03;
        byte >>= 2;
    }
}

bool Ppu::frame_is_ready(void)
{
    return frame_ready;
}

u8* Ppu::get_viewport(void)
{
    frame_ready = false;
    return reinterpret_cast<u8*>(viewport.data());
}

u8* Ppu::get_vram(void)
{
    return vram.data();
}

u8* Ppu::get_oam_ram(void)
{
    return oam_ram.data();
}

u8* Ppu::get_bg_map(void)
{
    return reinterpret_cast<u8*>(bg_map.data());
}

u8* Ppu::get_win_map(void)
{
    return reinterpret_cast<u8*>(win_map.data());
}
};
