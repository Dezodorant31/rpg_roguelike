#pragma once

#include "world/Map.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

// Рисует видимую часть карты.
// Один RectangleShape переиспользуется для всех тайлов —
// это дешевле, чем создавать/уничтожать объект на каждый тайл.
class RenderSystem
{
public:
    explicit RenderSystem(int tileSize)
        : tileSize_(tileSize)
    {
        shape_.setSize({static_cast<float>(tileSize),
                        static_cast<float>(tileSize)});
    }

    // cameraX/Y — мировые координаты левого верхнего угла экрана.
    void drawMap(sf::RenderWindow& window,
                 const Map& map,
                 float cameraX, float cameraY) const
    {
        // Какие тайлы видны?
        const int firstX = static_cast<int>(cameraX) / tileSize_;
        const int firstY = static_cast<int>(cameraY) / tileSize_;

        const auto size = window.getSize();
        const int lastX = firstX + static_cast<int>(size.x) / tileSize_ + 1;
        const int lastY = firstY + static_cast<int>(size.y) / tileSize_ + 1;

        // Клампим в границы карты
        const int x0 = std::max(0, firstX);
        const int y0 = std::max(0, firstY);
        const int x1 = std::min(map.width(),  lastX);
        const int y1 = std::min(map.height(), lastY);

        for (int y = y0; y < y1; ++y)
        {
            for (int x = x0; x < x1; ++x)
            {
                const auto& tile = map.at(x, y);

                shape_.setPosition({
                    static_cast<float>(x * tileSize_) - cameraX,
                    static_cast<float>(y * tileSize_) - cameraY
                });
                shape_.setFillColor(tile.color);
                window.draw(shape_);
            }
        }
    }

private:
    int tileSize_;
    mutable sf::RectangleShape shape_;   // mutable: метод const, но фигуру меняем
};
