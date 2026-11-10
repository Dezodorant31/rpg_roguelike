#pragma once

#include "world/Tile.hpp"

#include <cstdint>
#include <span>
#include <vector>

// Плоская карта. Одна аллокация, O(1) доступ по (x, y).
class Map
{
public:
    Map(int width, int height);

    [[nodiscard]] int width()  const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }

    // Проверка: попадает ли (x, y) в границы карты?
    // Любая операция доступа ДОЛЖНА сначала спросить это.
    [[nodiscard]] bool inBounds(int x, int y) const noexcept
    {
        return x >= 0 && y >= 0 && x < width_ && y < height_;
    }

    // Доступ к клетке. БЕЗ проверки границ — быстрый путь.
    // Вызывающий обязан сам проверить inBounds().
    [[nodiscard]] Tile&       at(int x, int y)       noexcept { return tiles_[index(x, y)]; }
    [[nodiscard]] const Tile& at(int x, int y) const noexcept { return tiles_[index(x, y)]; }

    // Безопасный доступ: возвращает false, если за границей.
    // Удобно для FOV и проверок ходьбы.
    [[nodiscard]] bool tryAt(int x, int y, Tile& out) const noexcept
    {
        if (!inBounds(x, y)) return false;
        out = tiles_[index(x, y)];
        return true;
    }

    // Можно ли ходить по клетке? false, если за границей или стена.
    [[nodiscard]] bool isWalkable(int x, int y) const noexcept
    {
        if (!inBounds(x, y)) return false;
        return tiles_[index(x, y)].isWalkable();
    }

    // Заполнить всю карту одним типом (для тестов/отладки)
    void fill(TileType type);

    // Прямой доступ к данным — для рендера и систем
    [[nodiscard]] std::span<const Tile> tiles() const noexcept { return tiles_; }
    [[nodiscard]] std::span<Tile>       tiles()       noexcept { return tiles_; }

private:
    [[nodiscard]] std::size_t index(int x, int y) const noexcept
    {
        return static_cast<std::size_t>(y) * static_cast<std::size_t>(width_)
             + static_cast<std::size_t>(x);
    }

    int width_  = 0;
    int height_ = 0;
    std::vector<Tile> tiles_;
};
