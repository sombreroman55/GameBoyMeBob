// Main ImGui renderer
// Probably will get rather large

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_memory_editor.h"
#include <SDL.h>
#include <cstdio>
#include <memory>
#include <vector>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#include "cartridge.hh"
#include "gb.hh"
using namespace gameboymebob;

// Globals so we can easily inspect them
Cartridge* cart = nullptr;
GameBoy* gb = nullptr;

//                        #9BBC0F,    #8BAC0F,    #306230,    #0F380F,    #11DDAA
uint32_t colors[5] = { 0xFF9BBC0F, 0xFF8BAC0F, 0xFF306230, 0xFF0F380F, 0xFF11DDAA };

constexpr int FRAMERATE_MS = 1000 / 60;
constexpr int SCREEN_HEIGHT = 144;
constexpr int SCREEN_WIDTH = 160;

void draw_viewport(void)
{
    // Need to get frame no matter what to reset frame ready flag
    u8* frame = gb->get_frame();
    static bool show_viewport = true;
    static int scale = 1;
    static GLuint image_texture = 0;
    if (image_texture == 0) {
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    if (show_viewport) {
        // map values to colors
        int scaled_width = SCREEN_WIDTH * scale;
        int scaled_height = SCREEN_HEIGHT * scale;
        u32 pixels[scaled_height * scaled_width];
        for (int i = 0; i < scaled_height; i++) {
            for (int j = 0; j < scaled_width; j++) {
                pixels[i * scaled_width + j] = colors[frame[(i/scale) * SCREEN_WIDTH + (j/scale)]];
            }
        }

        // render colors to texture
        // Create a OpenGL texture identifier
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, scaled_width, scaled_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

        // render texture to window
        const char* title = cart->header.title.c_str();
        if (!title || title[0] == '\0') {
            title = "Viewport";
        }
        ImGui::Begin(title, &show_viewport);
        ImGui::SliderInt("Scale Factor", &scale, 1, 4);
        ImGui::Image((ImTextureID)(intptr_t)image_texture, ImVec2(scaled_width, scaled_height));
        ImGui::End();
    }
}

void draw_background(void)
{
    static bool draw_background = false;
    if (draw_background) {
    }
}

void draw_tile_data(void)
{
    static bool show_tile_data = false;
    if (show_tile_data) {
    }
}

void draw_cpu(void)
{
    static bool show_cpu = true;
    if (show_cpu) {
        Cpu* cpu = gb->get_cpu();
        ImGui::Begin("CPU", &show_cpu);
        ImGui::Text("A: %02X | F: %02X | AF: %04X)", cpu->reg->a, cpu->reg->f, cpu->reg->af);
        ImGui::Text("B: %02X | C: %02X | BC: %04X)", cpu->reg->b, cpu->reg->c, cpu->reg->bc);
        ImGui::Text("D: %02X | E: %02X | DE: %04X)", cpu->reg->d, cpu->reg->e, cpu->reg->de);
        ImGui::Text("H: %02X | L: %02X | HL: %04X)", cpu->reg->h, cpu->reg->l, cpu->reg->hl);
        ImGui::Text("SP: %04X", cpu->reg->sp);
        ImGui::Text("PC: %04X", cpu->reg->pc);
        ImGui::Text("Z: %d | N: %d | H: %d | C: %d",
                cpu->reg->flag_is_set(Flag::ZERO),
                cpu->reg->flag_is_set(Flag::NEG),
                cpu->reg->flag_is_set(Flag::HALF),
                cpu->reg->flag_is_set(Flag::CARRY));
        ImGui::End();
    }
}

void draw_memory(void)
{
    static bool show_memory = true;
    static MemoryEditor mem_editor;
    if (show_memory) {
        mem_editor.DrawWindow("Memory", gb->get_memory(), 0x10000);
    }
}

void draw_vram(void)
{
    static bool show_vram = false;
    static MemoryEditor vram_editor;
    if (show_vram) {
        vram_editor.DrawWindow("VRAM", gb->get_vram(), 0x2000);
    }
}

void draw_oam_ram(void)
{
    static bool show_oam_ram = false;
    if (show_oam_ram) {
        // u8* vram = gb->get_vram();
        u8* oam = gb->get_oam_ram();
        ImGui::Begin("OAM", &show_oam_ram);
        for (int i = 0; i < 40; i++) {
            int idx = i * 4;
            uint8_t ypos = oam[idx + 0];
            uint8_t xpos = oam[idx + 1];
            uint8_t tile = oam[idx + 2];
            uint8_t flag = oam[idx + 3];
            char buf[64];
            ImGui::Text("Y: %3d | X: %3d | T: %3d", ypos, xpos, tile);
        }
        ImGui::End();
    }
}

int main(int argc, char* argv[])
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("GameBoyMeBob", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);

    std::chrono::steady_clock::time_point frame_start;
    std::chrono::steady_clock::time_point frame_end;

    gb = new GameBoy();
    std::string rom_file(argv[1]);
    cart = new Cartridge(rom_file);
    gb->insert_cartridge(cart);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // handle input
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            gb->press_button(Button::Right);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            gb->press_button(Button::Left);
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            gb->press_button(Button::Up);
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            gb->press_button(Button::Down);
        if (ImGui::IsKeyPressed(ImGuiKey_Z))
            gb->press_button(Button::B);
        if (ImGui::IsKeyPressed(ImGuiKey_X))
            gb->press_button(Button::A);
        if (ImGui::IsKeyPressed(ImGuiKey_Enter))
            gb->press_button(Button::Start);
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace))
            gb->press_button(Button::Select);

        if (ImGui::IsKeyReleased(ImGuiKey_RightArrow))
            gb->release_button(Button::Right);
        if (ImGui::IsKeyReleased(ImGuiKey_LeftArrow))
            gb->release_button(Button::Left);
        if (ImGui::IsKeyReleased(ImGuiKey_UpArrow))
            gb->release_button(Button::Up);
        if (ImGui::IsKeyReleased(ImGuiKey_DownArrow))
            gb->release_button(Button::Down);
        if (ImGui::IsKeyReleased(ImGuiKey_Z))
            gb->release_button(Button::B);
        if (ImGui::IsKeyReleased(ImGuiKey_X))
            gb->release_button(Button::A);
        if (ImGui::IsKeyReleased(ImGuiKey_Enter))
            gb->release_button(Button::Start);
        if (ImGui::IsKeyReleased(ImGuiKey_Backspace))
            gb->release_button(Button::Select);

        frame_start = std::chrono::steady_clock::now();
        while (!gb->frame_ready()) {
            gb->run();
        }
        frame_end = std::chrono::steady_clock::now();

        auto time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start).count();
        if (time_spent < FRAMERATE_MS) {
            uint32_t sleep_ms = FRAMERATE_MS - time_spent;
            SDL_Delay(sleep_ms);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        draw_viewport();
        draw_background();
        draw_tile_data();
        draw_cpu();
        draw_memory();
        draw_vram();
        draw_oam_ram();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
