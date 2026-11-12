#pragma once

#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MessageLog.hpp"
#include "systems/MovementSystem.hpp"
#include "world/Map.hpp"

#include <cstdlib>

// Один ход монстра. Возвращает сообщение (может быть пустым).
inline std::string enemyTakeTurn(World& world, Map& map,
                                 EntityId monster, EntityId player,
                                 MessageLog& log)
{
    if (!world.isAlive(monster) || !world.isAlive(player)) return {};

    auto* mpos = world.get<Position>(monster);
    auto* ppos = world.get<Position>(player);
    if (!mpos || !ppos) return {};

    const int dx = ppos->x - mpos->x;
    const int dy = ppos->y - mpos->y;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);

    // Соседняя клетка? Атакуем.
    if (adx + ady == 1)
    {
        const std::string msg = dealDamage(world, player, 1);
        log.push(msg);
        return msg;
    }

    // Идём к игроку, обходя стены и других монстров.
    const int stepX = (dx > 0) - (dx < 0);
    const int stepY = (dy > 0) - (dy < 0);

    if (adx >= ady)
    {
        if (tryMove(world, map, monster, stepX, 0)) return {};
        tryMove(world, map, monster, 0, stepY);
    }
    else
    {
        if (tryMove(world, map, monster, 0, stepY)) return {};
        tryMove(world, map, monster, stepX, 0);
    }
    return {};
}
