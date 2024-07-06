#include "util.hpp"

#include <algorithm>

Rect getInnerRect(Rect parent, float aspect_ratio) {
    int height, width;
    if ((float)parent.w / parent.h > aspect_ratio) {
        height = std::min(parent.h, int(parent.w / aspect_ratio));
        width = int(height * aspect_ratio);
    } else {
        width = std::min(parent.w, int(parent.h * aspect_ratio));
        height = int(width / aspect_ratio);
    }

    Rect board = {parent.x + (parent.w - width) / 2, parent.y + (parent.h - height) / 2, width, height};

    return board;
}

