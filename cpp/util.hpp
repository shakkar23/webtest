#pragma once

struct Rect {
   public:
    int x, y, w, h;
};

Rect getInnerRect(Rect parent, float aspect_ratio);