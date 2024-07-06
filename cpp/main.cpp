// clang -Os -fno-builtin  --no-standard-libraries -Wl,--export=add -Wl,--no-entry --target=wasm32 main.c -o main.wasm
#include "Tetris/VSGame.hpp"
#include "util.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include "game_logic.hpp"

State state;
// window.keyboard state

// screen state state;

extern "C" void EMSCRIPTEN_KEEPALIVE keyup(char key) {
    state.keyboard.erase(key);
}
extern "C" void EMSCRIPTEN_KEEPALIVE keydown(char key) {
    state.keyboard.insert(key);
}

extern "C" uint8_t* EMSCRIPTEN_KEEPALIVE update(float dt, int w, int h) {
    if (state.win_w != w || state.win_h != h) {
        state.screen_data.clear();
        state.screen_data.resize(w * h * sizeof(uint32_t));
        state.win_w = w;
        state.win_h = h;
    }

    state.logic(dt);

    state.render();

    return (uint8_t*)state.screen_data.data();
}
