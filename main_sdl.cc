// Simple main file to start running Blargg files

#include "cartridge.hh"
#include "gb.hh"
#include <string>

#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* viewport_texture;

    constexpr int FRAMERATE_MS = 1000 / 60;
    constexpr int SCREEN_HEIGHT = 144;
    constexpr int SCREEN_WIDTH = 160;

    std::chrono::steady_clock::time_point frame_start;
    std::chrono::steady_clock::time_point frame_end;

    uint32_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    std::string window_title = "GameBoyMeBob";

    //                      #0F380F,  #306230,  #8BAC0F,  #9BBC0F,  #11DDAA };
    uint32_t colors[5] = { 0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F, 0x11DDAA };

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        window_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowTitle(window, window_title.c_str());

    const int FIRST_AVAILABLE_DRIVER = -1;
    renderer = SDL_CreateRenderer(
        window,
        FIRST_AVAILABLE_DRIVER,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    viewport_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    std::string rom_file(argv[1]);

    gameboymebob::GameBoy* gb = new gameboymebob::GameBoy();
    gameboymebob::Cartridge* cart = new gameboymebob::Cartridge(rom_file);
    gb->insert_cartridge(cart);

    frame_start = std::chrono::steady_clock::now();
    bool done = false;
    SDL_Event ev;
    while (!done) {
        gb->run();
        if (gb->frame_ready()) {
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) {
                    done = true;
                } else if (ev.type == SDL_KEYDOWN) {
                    gameboymebob::Button button = gameboymebob::Button::Invalid;
                    switch (ev.key.keysym.sym) {
                        // clang-format off
                    case SDLK_UP:           button = gameboymebob::Button::Up;      break;
                    case SDLK_DOWN:         button = gameboymebob::Button::Down;    break;
                    case SDLK_LEFT:         button = gameboymebob::Button::Left;    break;
                    case SDLK_RIGHT:        button = gameboymebob::Button::Right;   break;
                    case SDLK_x:            button = gameboymebob::Button::A;       break;
                    case SDLK_z:            button = gameboymebob::Button::B;       break;
                    case SDLK_BACKSPACE:    button = gameboymebob::Button::Select;  break;
                    case SDLK_RETURN:       button = gameboymebob::Button::Start;   break;
                        // clang-format on
                    }
                    if (button != gameboymebob::Button::Invalid) {
                        printf("Pressing %c...\n", ev.key.keysym.sym);
                        gb->press_button(button);
                    }
                } else if (ev.type == SDL_KEYUP) {
                    gameboymebob::Button button = gameboymebob::Button::Invalid;
                    switch (ev.key.keysym.sym) {
                        // clang-format off
                    case SDLK_UP:           button = gameboymebob::Button::Up;      break;
                    case SDLK_DOWN:         button = gameboymebob::Button::Down;    break;
                    case SDLK_LEFT:         button = gameboymebob::Button::Left;    break;
                    case SDLK_RIGHT:        button = gameboymebob::Button::Right;   break;
                    case SDLK_x:            button = gameboymebob::Button::A;       break;
                    case SDLK_z:            button = gameboymebob::Button::B;       break;
                    case SDLK_BACKSPACE:    button = gameboymebob::Button::Select;  break;
                    case SDLK_RETURN:       button = gameboymebob::Button::Start;   break;
                        // clang-format on
                    }
                    if (button != gameboymebob::Button::Invalid) {
                        printf("Releasing %c...\n", ev.key.keysym.sym);
                        gb->release_button(button);
                    }
                }
            }

            frame_end = std::chrono::steady_clock::now();
            auto time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start).count();
            std::cout << "Frame took " << time_spent << " ms" << std::endl;
            if (time_spent < FRAMERATE_MS) {
                uint32_t sleep_ms = FRAMERATE_MS - time_spent;
                SDL_Delay(sleep_ms);
            }
            frame_start = std::chrono::steady_clock::now();

            uint8_t* frame = gb->get_frame();

            SDL_RenderClear(renderer);
            void* pixels_ptr;
            int pitch;
            SDL_LockTexture(viewport_texture, nullptr, &pixels_ptr, &pitch);

            uint32_t* pixels = static_cast<uint32_t*>(pixels_ptr);

            for (int i = 0; i < SCREEN_HEIGHT; i++) {
                for (int j = 0; j < SCREEN_WIDTH; j++) {
                    pixels[i * SCREEN_WIDTH + j] = colors[frame[i * SCREEN_WIDTH + j]];
                }
            }

            SDL_UnlockTexture(viewport_texture);
            SDL_RenderCopy(renderer, viewport_texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    delete gb;
    SDL_DestroyTexture(viewport_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
