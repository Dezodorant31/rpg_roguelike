#pragma once

#include <SFML/Graphics/Color.hpp>

// Одна клетка карты. Пока простая — потом добавим:
//  - explored/visible (для FOV)
//  - тип поверхности (трава, камень, вода)
//  - ссылку на сущность (монстр, предмет) — но это уже через компоненты
enum class TileType : std::uint8_t
{
    Floor,   // пол — можно ходить
    Wall,    // стена — нельзя
};

struct Tile
{
    TileType type = TileType::Wall;

    // Цвет для отладки/рендера. Потом заменим на спрайт.
    sf::Color color = sf::Color::Black;

    // Можно ли по этой клетке ходить?
    [[nodiscard]] bool isWalkable() const noexcept
    {
        return type == TileType::Floor;
    }
};

// Фабрики — чтобы не дублировать цвет в коде
[[nodiscard]] inline Tile makeFloor()
{
    return Tile{ TileType::Floor, sf::Color(60, 60, 70) };
}

[[nodiscard]] inline Tile makeWall()
{
    return Tile{ TileType::Wall, sf::Color(30, 30, 35) };
}
