#pragma once

#include "world/Map.hpp"
#include "world/Rect.hpp"

#include <functional>
#include <string>
#include <vector>

// Prototype: заготовка комнаты, которую можно копировать в карту.
//
// Идея: заранее нарисованные шаблоны (сокровищница, алтарь, ловушка).
// При генерации билдер может «вставить» шаблон вместо случайной комнаты.
//
// Пока не используется в BSP, но интерфейс готов.
struct RoomTemplate
{
    std::string name;
    int width = 0;
    int height = 0;

    // Клетки относительно (0, 0)
    // '#' — стена, '.' — пол, '+' — дверь (пол, но с меткой)
    std::vector<std::string> layout;

    // Применить шаблон в карту по позиции (x, y) — левый верхний угол
    void applyTo(Map& map, int x, int y) const
    {
        for (int dy = 0; dy < height && dy < static_cast<int>(layout.size()); ++dy)
        {
            const auto& row = layout[static_cast<std::size_t>(dy)];
            for (int dx = 0; dx < width && dx < static_cast<int>(row.size()); ++dx)
            {
                const char c = row[static_cast<std::size_t>(dx)];
                if (!map.inBounds(x + dx, y + dy)) continue;

                if (c == '#')      map.at(x + dx, y + dy) = makeWall();
                else if (c == '.') map.at(x + dx, y + dy) = makeFloor();
                else if (c == '+') map.at(x + dx, y + dy) = makeFloor();  // дверь
            }
        }
    }
};
