// clang -Os -fno-builtin  --no-standard-libraries -Wl,--export=add -Wl,--no-entry --target=wasm32 main.c -o main.wasm
#include "Tetris/VSGame.hpp"
#include "util.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>
// keyboard state
std::set<char> keyboard;

// screen state
class Window {
   public:
    struct Color {
        uint8_t r, g, b, a;
    };
    std::vector<Color> screen_data;
    int win_w{}, win_h{};

    void DrawRect(Rect outline, Color c) {
        if (0 <= outline.x && outline.x <= win_w && outline.y < win_h) {
            for (int i = outline.x; i < outline.x + outline.w; ++i) {
                int x = std::clamp(i, 0, win_w - 1);
                int y1 = std::clamp(outline.y, 0, win_h - 1);
                int y2 = std::clamp(outline.h + outline.y, 0, win_h - 1);

                // draw the two horizontal lines
                screen_data[y1 * win_w + x].r = c.r;
                screen_data[y1 * win_w + x].g = c.g;
                screen_data[y1 * win_w + x].b = c.b;
                screen_data[y1 * win_w + x].a = c.a;

                screen_data[y2 * win_w + x].r = c.r;
                screen_data[y2 * win_w + x].g = c.g;
                screen_data[y2 * win_w + x].b = c.b;
                screen_data[y2 * win_w + x].a = c.a;
            }
        }

        if (0 <= outline.y && outline.y < win_h && outline.x < win_w) {
            for (int i = outline.y; i <= outline.y + outline.h; ++i) {
                int y = std::clamp(i, 0, win_h - 1);

                int x1 = std::clamp(outline.x, 0, win_w - 1);
                int x2 = std::clamp(outline.w + outline.x, 0, win_w - 1);

                // draw the two vertical lines
                screen_data[y * win_w + x1].r = c.r;
                screen_data[y * win_w + x1].g = c.g;
                screen_data[y * win_w + x1].b = c.b;
                screen_data[y * win_w + x1].a = c.a;

                if (outline.x + outline.w < win_w) {
                    screen_data[y * win_w + x2].r = c.r;
                    screen_data[y * win_w + x2].g = c.g;
                    screen_data[y * win_w + x2].b = c.b;
                    screen_data[y * win_w + x2].a = c.a;
                }
            }
        }
    }

    void DrawRectFilled(Rect outline, Color c) {
        outline.x = std::clamp(outline.x, 0, win_w - 1);
        outline.y = std::clamp(outline.y, 0, win_h - 1);
        outline.w = std::clamp(outline.w, 0, win_w - outline.x);
        outline.h = std::clamp(outline.h, 0, win_h - outline.y);

        // render
        for (int y = outline.y; y <= outline.y + outline.h; ++y) {
            for (int x = outline.x; x <= outline.x + outline.w; ++x) {
                screen_data[y * win_w + x].r = c.r;
                screen_data[y * win_w + x].g = c.g;
                screen_data[y * win_w + x].b = c.b;
                screen_data[y * win_w + x].a = c.a;
            }
        }
    }
} window;
Window::Color red = {.r = 255, .a = 255};
Window::Color green = {.g = 255, .a = 255};
Window::Color blue = {.b = 255, .a = 255};
Window::Color gray = {.r = 150, .g = 150, .b = 150, .a = 255};
// game state
VersusGame game;

extern "C" void EMSCRIPTEN_KEEPALIVE keyup(char key) {
    keyboard.erase(key);
}
extern "C" void EMSCRIPTEN_KEEPALIVE keydown(char key) {
    keyboard.insert(key);
}

extern "C" uint8_t* EMSCRIPTEN_KEEPALIVE update(float dt, int w, int h) {
    if (window.win_w != w || window.win_h != h) {
        window.screen_data.clear();
        window.screen_data.resize(w * h * sizeof(uint32_t));
        window.win_w = w;
        window.win_h = h;
    }

    if (std::find(keyboard.begin(), keyboard.end(), 'w') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::SonicDrop);
        Move move = Move(game.p1_game.current_piece, false);
        Move null_move = Move(game.p2_game.current_piece, true);
        game.set_move(0, move);
        game.set_move(1, null_move);
        game.play_moves();
        keyboard.erase('w');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 'a') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::Left);
        keyboard.erase('a');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 's') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::SonicDrop);
        keyboard.erase('s');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 'd') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::Right);
        keyboard.erase('d');
    }

    if (std::find(keyboard.begin(), keyboard.end(), 'W') != keyboard.end()) {
        // do hold
        game.p1_game.do_hold();
        keyboard.erase('W');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 'A') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::RotateCounterClockwise);
        keyboard.erase('A');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 'S') != keyboard.end()) {
        // we dont support 180s yet
        keyboard.erase('S');
    }
    if (std::find(keyboard.begin(), keyboard.end(), 'D') != keyboard.end()) {
        game.p1_game.process_movement(game.p1_game.current_piece, Movement::RotateClockwise);
        keyboard.erase('D');
    }

    // clear
    for (std::size_t i = 0; i < window.screen_data.size(); i += 1) {
        window.screen_data[i].r = 0;    // R value
        window.screen_data[i].g = 0;    // G value
        window.screen_data[i].b = 0;    // B value
        window.screen_data[i].a = 255;  // A value
    }

    Rect resting_eye_location = getInnerRect({.x = 0, .y = 0, .w = window.win_w, .h = window.win_h}, 1.0 / 1.0);
    // move to where the eyes rest according to the thirds rule
    // we will render everything in here
    resting_eye_location.y = int((resting_eye_location.y - 0) * 0.33);

    const Rect board_area = getInnerRect(resting_eye_location, 10.0 / 20.0);

    // render the current piece
    for (auto& mino : game.p1_game.current_piece.minos) {
        int x = mino.x + game.p1_game.current_piece.position.x;
        int y = 19 - (mino.y + game.p1_game.current_piece.position.y);

        float cell_length = board_area.w / 10.0;
        int cell_x = board_area.x + cell_length * x;
        int cell_y = board_area.y + cell_length * y;
        window.DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, blue);
    }

    // render the board cells
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 20; ++y) {
            float cell_length = board_area.w / 10.0;
            int cell_x = board_area.x + cell_length * x;
            int cell_y = board_area.y + cell_length * (19 - y);
            if (game.p1_game.board.get(x, y))
                window.DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, gray);
        }

    // render the board outline
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 20; ++y) {
            float cell_length = board_area.w / 10.0;
            int cell_x = board_area.x + cell_length * x;
            int cell_y = board_area.y + cell_length * y;
            window.DrawRect({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, gray);
        }

    return (uint8_t*)window.screen_data.data();
}
