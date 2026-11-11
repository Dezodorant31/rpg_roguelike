#pragma once

#include "components/Health.hpp"
#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "world/Map.hpp"

#include <cstdlib>
#include <print>

// Простейший AI: один шаг к игроку по оси с наибольшей разницей.
// Если игрок в соседней клетке — атакует.
//
// Это ЗАГЛУШКА. В Спринте 7 заменим на Strategy + Type Object:
// - AggressiveBehavior: идёт и бьёт
// - CowardBehavior: убегает, если HP мало
// - PatrolBehavior: ходит по маршруту
inline void enemyTakeTurn(World& world, Map& map,
                          EntityId monster, EntityId player)
{
    if (!world.isAlive(monster) || !world.isAlive(player)) return;

    auto* mpos = world.get<Position>(monster);
    auto* ppos = world.get<Position>(player);
    if (!mpos || !ppos) return;

    const int dx = ppos->x - mpos->x;
    const int dy = ppos->y - mpos->y;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);

    // Соседняя клетка? Атакуем.
    if (adx + ady == 1)
    {
        std::println("{}", dealDamage(world, player, 1));
        return;
    }

    // Идём по оси с большей разницей. При равенстве — по X.
    int stepX = (dx > 0) - (dx < 0);   // знак dx, без ветвлений
    int stepY = (dy > 0) - (dy < 0);

    if (adx >= ady)
    {
        if (tryMove(world, map, monster, stepX, 0)) return;
        tryMove(world, map, monster, 0, stepY);
    }
    else
    {
        if (tryMove(world, map, monster, 0, stepY)) return;
        tryMove(world, map, monster, stepX, 0);
    }
}
