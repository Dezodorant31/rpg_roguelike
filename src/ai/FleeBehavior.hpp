#pragma once

#include "ai/Behavior.hpp"
#include "systems/MovementSystem.hpp"

#include <cstdlib>

// ---------------------------------------------------------------------------
// FleeBehavior — убегает от игрока, если тот близко.
//
// Логика: если игрок в радиусе vision — двигаемся ОТ него (противоположное
// направление). Если далеко — стоим (крыса не патрулирует).
//
// Почему «от игрока», а не «к ближайшему выходу»:
//   - проще; для крысы этого достаточно;
//   - поиск пути (A*) — отдельная задача, добавим в Спринте 11, если надо.
// ---------------------------------------------------------------------------
class FleeBehavior final : public Behavior
{
public:
    void takeTurn(World& world, Map& map,
                  EntityId self, EntityId player,
                  const MonsterType& type,
                  MessageLog& log) const override
    {
        (void)log;   // трус не логирует
        if (!world.isAlive(self) || !world.isAlive(player)) return;

        auto* mpos = world.get<Position>(self);
        auto* ppos = world.get<Position>(player);
        if (!mpos || !ppos) return;

        const int dx = ppos->x - mpos->x;
        const int dy = ppos->y - mpos->y;
        const int adx = std::abs(dx);
        const int ady = std::abs(dy);

        // Далеко — стоим.
        if (adx + ady > type.visionRange) return;

        // Убегаем: шаг ПРОТИВ направления к игроку.
        const int stepX = (dx > 0) - (dx < 0);
        const int stepY = (dy > 0) - (dy < 0);

        // Сначала пробуем «главную» ось (по которой дальше), потом вторую.
        if (adx >= ady)
        {
            if (tryMove(world, map, self, -stepX, 0)) return;
            tryMove(world, map, self, 0, -stepY);
        }
        else
        {
            if (tryMove(world, map, self, 0, -stepY)) return;
            tryMove(world, map, self, -stepX, 0);
        }
    }

    [[nodiscard]] const char* name() const override { return "Flee"; }
};
