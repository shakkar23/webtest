// clang -Os -fno-builtin  --no-standard-libraries -Wl,--export=add -Wl,--no-entry --target=wasm32 main.c -o main.wasm

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <cstddef>
#include <cstdint>
#include <vector>

std::vector<uint8_t> screen_data;
int win_w{}, win_h{};

int value = 190;

extern "C" uint8_t* EMSCRIPTEN_KEEPALIVE render(int w, int h) {
    if (win_w != w || win_h != h) {
        screen_data.clear();
        screen_data.resize(w * h * sizeof(uint32_t));
        win_w = w;
        win_h = h;
    }

    for (std::size_t i = 0; i < screen_data.size(); i += 4) {
        screen_data[i + 0] = 0;            // R value
        screen_data[i + 1] = value % 255;  // G value
        screen_data[i + 2] = 0;            // B value
        screen_data[i + 3] = 255;          // A value
    }

    value++;

    return screen_data.data();
}
