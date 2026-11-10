#pragma once

#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "world/Map.hpp"

// Пытается сдвинуть сущность на (dx, dy).
// Возвращает true, если сдвиг удался (клетка проходима).
//
// Это ОБЩИЙ код движения — используется и командами игрока, и AI монстров.
inline bool tryMove(World& world, Map& map, EntityId id, int dx, int dy)
{
    auto* pos = world.get<Position>(id);
    if (!pos) return false;

    const int nx = pos->x + dx;
    const int ny = pos->y + dy;

    if (!map.isWalkable(nx, ny)) return false;

    pos->x = nx;
    pos->y = ny;
    return true;
}
