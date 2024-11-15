#include "joypad.hh"

namespace gameboymebob {
Joypad::Joypad(Mmu* mmu, InterruptController* ic)
    : mem(mmu)
    , interrupts(ic)
    , joyp(&mem->memory[IoRegisters::joyp])
    , button_gates(0xFF)
{
    *joyp = 0xCF;
    mem->map_joyad(this);
}

bool Joypad::opposing_directions_pressed(Button button)
{
    // clang-format off
    if (button == Button::Right) return (button_gates & 0x02) == 0;
    if (button == Button::Left)  return (button_gates & 0x01) == 0;
    if (button == Button::Up)    return (button_gates & 0x08) == 0;
    if (button == Button::Down)  return (button_gates & 0x04) == 0;
    return false;
    // clang-format on
}

void Joypad::press_button(Button button)
{
    if (opposing_directions_pressed(button)) {
        // This is not allowed, just return
        return;
    }

    u8 offset = 1 << static_cast<u8>(button);
    u8 prev_state = button_gates;
    u8 newly_pressed = (prev_state & offset) == offset; // button is not pressed

    u8 select_state = (*joyp & 0x30) >> 4;
    u8 is_direction = static_cast<u8>(button) < 4;

    // Generate interrupt if select bits are reset and button
    // in that select group is being newly pressed
    bool group_selected = (is_direction && (select_state & 0x01) == 0) || (!is_direction && (select_state < 0x02));
    bool generate_interrupt = group_selected && newly_pressed;

    button_gates &= ~offset; // reset bit to press button
    if (generate_interrupt) {
        interrupts->set_interrupt(Interrupt::JOYPAD);
    }
}

void Joypad::release_button(Button button)
{
    // set bit to release button
    button_gates |= (1 << static_cast<u8>(button));
}

u8 Joypad::read_buttons(void)
{
    return (0xF0 | ((button_gates >> 4) & 0x0F));
}

u8 Joypad::read_directions(void)
{
    return (0xF0 | (button_gates & 0x0F));
}

void Joypad::write_button_register(u8 b)
{
    *joyp &= ~(b & 0x30);
}

u8 Joypad::read_button_register(void)
{
    u8 select_state = (*joyp & 0x30) >> 4;
    switch (select_state) {
    case 0x00: // both selected
        *joyp &= (read_buttons() | read_directions());
        break;
    case 0x01: // buttons
        *joyp &= read_buttons();
        break;
    case 0x02: // dirs
        *joyp &= read_directions();
        break;
    case 0x03: // none selected
        *joyp |= 0x0F;
        break;
    }
    return *joyp;
}
};
