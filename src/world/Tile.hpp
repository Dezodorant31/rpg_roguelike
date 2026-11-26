#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>

enum class TileType : std::uint8_t
{
    Floor,
    Wall,
};

struct Tile
{
    TileType type = TileType::Wall;
    sf::Color color = sf::Color::Black;

    // ---- FOV-состояние (Спринт 6) ----
    // visible  — клетка видна игроку ПРЯМО СЕЙЧАС
    // explored — клетка была видна ХОТЯ БЫ РАЗ
    // Инвариант: visible ⇒ explored.
    bool visible  = false;
    bool explored = false;

    [[nodiscard]] bool isWalkable() const noexcept
    {
        return type == TileType::Floor;
    }
};

[[nodiscard]] inline Tile makeFloor()
{
    return Tile{ TileType::Floor, sf::Color(60, 60, 70) };
}

[[nodiscard]] inline Tile makeWall()
{
    return Tile{ TileType::Wall, sf::Color(30, 30, 35) };
}
