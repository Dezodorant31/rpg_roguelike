#pragma once

#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "ecs/World.hpp"
#include "world/Map.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <cstdint>

class RenderSystem
{
public:
    explicit RenderSystem(int tileSize)
        : tileSize_(tileSize)
    {
        shape_.setSize({static_cast<float>(tileSize),
                        static_cast<float>(tileSize)});
    }

    void drawMap(sf::RenderWindow& window,
                 const Map& map,
                 float cameraX, float cameraY) const
    {
        const int firstX = static_cast<int>(cameraX) / tileSize_;
        const int firstY = static_cast<int>(cameraY) / tileSize_;

        const auto size = window.getSize();
        const int lastX = firstX + static_cast<int>(size.x) / tileSize_ + 1;
        const int lastY = firstY + static_cast<int>(size.y) / tileSize_ + 1;

        const int x0 = std::max(0, firstX);
        const int y0 = std::max(0, firstY);
        const int x1 = std::min(map.width(),  lastX);
        const int y1 = std::min(map.height(), lastY);

        for (int y = y0; y < y1; ++y)
        {
            for (int x = x0; x < x1; ++x)
            {
                const auto& tile = map.at(x, y);

                sf::Color color;
                if (tile.visible)
                {
                    color = tile.color;
                }
                else if (tile.explored)
                {
                    color = sf::Color(
                        static_cast<std::uint8_t>(tile.color.r * 35 / 100),
                        static_cast<std::uint8_t>(tile.color.g * 35 / 100),
                        static_cast<std::uint8_t>(tile.color.b * 35 / 100),
                        tile.color.a);
                }
                else
                {
                    color = sf::Color(10, 10, 15);
                }

                shape_.setPosition({
                    static_cast<float>(x * tileSize_) - cameraX,
                    static_cast<float>(y * tileSize_) - cameraY
                });
                shape_.setFillColor(color);
                window.draw(shape_);
            }
        }
    }

    // Рисует сущности с Position + Renderable, но только на ВИДИМЫХ клетках.
    void drawEntities(sf::RenderWindow& window,
                      World& world,
                      const Map& map,
                      float cameraX, float cameraY)
    {
        for (auto [id, pos] : world.view<Position>())
        {
            auto* rend = world.get<Renderable>(id);
            if (!rend) continue;

            // FOV: рисуем только то, что на видимой клетке
            if (!map.inBounds(pos->x, pos->y)) continue;
            if (!map.at(pos->x, pos->y).visible) continue;

            const float s = static_cast<float>(tileSize_) * rend->sizeRatio;
            const float offset = (static_cast<float>(tileSize_) - s) / 2.f;

            shape_.setSize({s, s});
            shape_.setPosition({
                static_cast<float>(pos->x * tileSize_) - cameraX + offset,
                static_cast<float>(pos->y * tileSize_) - cameraY + offset
            });
            shape_.setFillColor(rend->color);
            window.draw(shape_);
        }

        // Восстанавливаем размер (drawMap ожидает полный тайл)
        shape_.setSize({static_cast<float>(tileSize_),
                        static_cast<float>(tileSize_)});
    }

private:
    int tileSize_;
    mutable sf::RectangleShape shape_;
};
