#pragma once

#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/CollisionSystem.hpp"
#include "world/Map.hpp"

// Попытаться сдвинуть сущность.
// Возвращает true, если сдвиг удался.
// false — если стена или клетка занята другой сущностью.
inline bool tryMove(World& world, Map& map, EntityId id, int dx, int dy)
{
    auto* pos = world.get<Position>(id);
    if (!pos) return false;

    const int nx = pos->x + dx;
    const int ny = pos->y + dy;

    if (!map.isWalkable(nx, ny)) return false;
    if (isOccupied(world, nx, ny, id)) return false;

    pos->x = nx;
    pos->y = ny;
    return true;
}
