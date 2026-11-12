#pragma once

#include <cstdint>

// Простой целочисленный прямоугольник.
// (x, y) — левый верхний угол. (w, h) — размер.
struct Rect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    [[nodiscard]] int right()  const noexcept { return x + w; }
    [[nodiscard]] int bottom() const noexcept { return y + h; }

    [[nodiscard]] int centerX() const noexcept { return x + w / 2; }
    [[nodiscard]] int centerY() const noexcept { return y + h / 2; }

    // Пересечение с другим прямоугольником?
    [[nodiscard]] bool intersects(const Rect& o) const noexcept
    {
        return x < o.right() && right() > o.x
            && y < o.bottom() && bottom() > o.y;
    }

    // Содержит точку?
    [[nodiscard]] bool contains(int px, int py) const noexcept
    {
        return px >= x && px < right() && py >= y && py < bottom();
    }

    // Содержит другой прямоугольник полностью?
    [[nodiscard]] bool contains(const Rect& o) const noexcept
    {
        return o.x >= x && o.y >= y && o.right() <= right() && o.bottom() <= bottom();
    }
};
