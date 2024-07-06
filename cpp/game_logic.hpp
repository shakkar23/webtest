#pragma once
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "Tetris/VSGame.hpp"
#include "util.hpp"

class State {
   public:
    struct Color {
        uint8_t r, g, b, a;
    };
    // POD
    struct Key {
        bool pressed;
        bool held;
        bool released;
    };

    enum ArrowCodes{
        Up, Left, Down, Right
    };

    std::set<char> keyboard, prev_keyboard;
    std::vector<Color> screen_data;
    int win_w{}, win_h{};
    VersusGame game;

    void DrawRect(Rect outline, Color c);
    void DrawRectFilled(Rect outline, Color c);

    void logic(float dt);
    void render();

   private:
    Key getKey(char key);
    bool justPressed(bool prevInput, bool input);
};

inline State::Color red = {.r = 255, .a = 255};
inline State::Color green = {.g = 255, .a = 255};
inline State::Color blue = {.b = 255, .a = 255};
inline State::Color gray = {.r = 150, .g = 150, .b = 150, .a = 255};
inline State::Color light_gray = {.r = 100, .g = 100, .b = 100, .a = 255};