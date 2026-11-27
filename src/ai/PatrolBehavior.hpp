#pragma once

#include "ai/Behavior.hpp"
#include "systems/MovementSystem.hpp"

#include <cstdlib>

// ---------------------------------------------------------------------------
// PatrolBehavior — патрулирует, атакует только если игрок рядом.
//
// «Патрулирование» здесь — просто «двигайся в случайном направлении,
// если игрок далеко». Настоящий маршрут (waypoints) — отдельная задача;
// для орка «бродит по комнате» достаточно.
//
// Почему случайное, а не по маршруту:
//   - маршрут требует хранения waypoints в компоненте — это Спринт 8+
//     (когда будет нормальный AI с состояниями);
//   - «бродит» уже даёт эффект «орк не стоит столбом».
// ---------------------------------------------------------------------------
class PatrolBehavior final : public Behavior
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

        // Игрок рядом — атакуем (как агрессивный).
        if (adx + ady == 1)
        {
            const std::string msg = dealDamage(world, player, type.damage);
            if (!msg.empty()) log.push(msg);
            return;
        }

        // Игрок в поле зрения — идём к нему (как агрессивный).
        if (adx + ady <= type.visionRange)
        {
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
            return;
        }

        // Игрок далеко — бродим в случайном направлении.
        // Простой псевдослучайный выбор на основе координат и «тика»:
        // не нужен std::mt19937, достаточно, чтобы орк не стоял.
        // (Если хочешь настоящий рандом — добавь RNG в параметры Behavior.)
        const int dir = (mpos->x + mpos->y) % 4;
        const int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        tryMove(world, map, self, dirs[dir][0], dirs[dir][1]);
    }

    [[nodiscard]] const char* name() const override { return "Patrol"; }
};
