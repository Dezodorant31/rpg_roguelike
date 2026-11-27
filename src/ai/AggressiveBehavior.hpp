#pragma once

#include "ai/Behavior.hpp"
#include "components/Health.hpp"
#include "components/Name.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MovementSystem.hpp"

#include <cstdlib>

// ---------------------------------------------------------------------------
// AggressiveBehavior — идёт к игроку, атакует в упор.
//
// Это ровно то, что делал старый enemyTakeTurn, но теперь:
//   - урон берётся из MonsterType (goblin.damage = 2, rat.damage = 1);
//   - есть visionRange: если игрок дальше — монстр стоит (не «телепат»).
// ---------------------------------------------------------------------------
class AggressiveBehavior final : public Behavior
{
public:
    void takeTurn(World& world, Map& map,
                  EntityId self, EntityId player,
                  const MonsterType& type,
                  MessageLog& log) const override
    {
        if (!world.isAlive(self) || !world.isAlive(player)) return;

        auto* mpos = world.get<Position>(self);
        auto* ppos = world.get<Position>(player);
        if (!mpos || !ppos) return;

        const int dx = ppos->x - mpos->x;
        const int dy = ppos->y - mpos->y;
        const int adx = std::abs(dx);
        const int ady = std::abs(dy);

        // Игрок слишком далеко — не замечаем.
        if (adx + ady > type.visionRange) return;

        // В упор — атакуем.
        if (adx + ady == 1)
        {
            const std::string msg = dealDamage(world, player, type.damage);
            if (!msg.empty()) log.push(msg);
            return;
        }

        // Идём к игроку.
        const int stepX = (dx > 0) - (dx < 0);
        const int stepY = (dy > 0) - (dy < 0);

        if (adx >= ady)
        {
            if (tryMove(world, map, self, stepX, 0)) return;
            tryMove(world, map, self, 0, stepY);
        }
        else
        {
            if (tryMove(world, map, self, 0, stepY)) return;
            tryMove(world, map, self, stepX, 0);
        }
    }

    [[nodiscard]] const char* name() const override { return "Aggressive"; }
};
