#pragma once

#include "world/Map.hpp"

// ---------------------------------------------------------------------------
// FOVSystem — поле зрения игрока.
//
// Алгоритм: symmetric shadowcasting (8 октантов).
// Для каждого октанта рекурсивно сканируем лучи от источника,
// пока не упрёмся в стены. Стена сама видима (луч дошёл до неё),
// но за ней — темно.
// ---------------------------------------------------------------------------
class FOVSystem
{
public:
    static void compute(Map& map, int originX, int originY, int radius)
    {
        for (auto& t : map.tiles())
            t.visible = false;

        if (map.inBounds(originX, originY))
        {
            auto& t = map.at(originX, originY);
            t.visible  = true;
            t.explored = true;
        }

        static constexpr int octants[8][4] = {
            { 1,  0,  0,  1},
            { 0,  1,  1,  0},
            { 0, -1,  1,  0},
            {-1,  0,  0,  1},
            {-1,  0,  0, -1},
            { 0, -1, -1,  0},
            { 0,  1, -1,  0},
            { 1,  0,  0, -1},
        };

        for (const auto& oct : octants)
        {
            castLight(map, originX, originY, radius,
                      /*row=*/1,
                      /*startSlope=*/1.0,      // ← было -1.0
                      /*endSlope=*/-1.0,       // ← было 1.0
                      oct[0], oct[1], oct[2], oct[3]);
        }
    }

private:
    static void castLight(Map& map,
                          int originX, int originY,
                          int radius,
                          int row,
                          double startSlope,
                          double endSlope,
                          int xx, int xy, int yx, int yy)
    {
        // Интервал валиден, когда startSlope >= endSlope.
        // startSlope — «левый» (больший) наклон, endSlope — «правый» (меньший).
        if (startSlope < endSlope) return;

        double newStart = startSlope;

        for (int dx = row; dx <= radius; ++dx)
        {
            bool blocked = false;

            for (int dy = -dx; dy <= 0; ++dy)
            {
                const int worldX = originX + xx * dx + yx * dy;
                const int worldY = originY + xy * dx + yy * dy;

                const double leftSlope  = (dy - 0.5) / (dx + 0.5);
                const double rightSlope = (dy + 0.5) / (dx - 0.5);

                // Клетка левее интервала — пропускаем.
                if (leftSlope > newStart) continue;
                // Клетка правее интервала — ряд закончился.
                if (rightSlope < endSlope) break;

                if (map.inBounds(worldX, worldY))
                {
                    auto& t = map.at(worldX, worldY);
                    t.visible  = true;
                    t.explored = true;

                    if (!t.isWalkable())
                    {
                        if (!blocked)
                        {
                            blocked  = true;
                            newStart = rightSlope;
                        }
                    }
                    else if (blocked)
                    {
                        castLight(map, originX, originY, radius,
                                  dx + 1, newStart, leftSlope,
                                  xx, xy, yx, yy);
                        newStart = rightSlope;
                        blocked  = false;
                    }
                }
            }

            if (blocked) break;
        }
    }
};
