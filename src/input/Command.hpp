#pragma once

#include "components/Health.hpp"
#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MessageLog.hpp"
#include "systems/MovementSystem.hpp"
#include "world/Map.hpp"

#include <optional>

struct GameContext
{
    World*       world = nullptr;
    Map*         map   = nullptr;
    MessageLog*  log   = nullptr;
    EntityId     player;
};

class Command
{
public:
    virtual ~Command() = default;
    virtual bool execute(GameContext& ctx) = 0;
};

// ---- Движение с auto-attack ----
//
// Если в целевой клетке монстр — атакуем.
// Если стена — не двигаемся, ход НЕ потрачен.
// Если пусто — двигаемся, ход потрачен.
class MoveCommand final : public Command
{
public:
    explicit MoveCommand(int dx, int dy) noexcept : dx_(dx), dy_(dy) {}

    bool execute(GameContext& ctx) override
    {
        if (!ctx.world || !ctx.map) return false;
        if (!ctx.world->isAlive(ctx.player)) return false;

        auto* ppos = ctx.world->get<Position>(ctx.player);
        if (!ppos) return false;

        const int tx = ppos->x + dx_;
        const int ty = ppos->y + dy_;

        // Есть монстр? Атакуем.
        if (auto target = monsterAt(*ctx.world, tx, ty); target)
        {
            const std::string msg = dealDamage(*ctx.world, *target, 3);
            if (ctx.log) ctx.log->push(msg);

            if (auto* h = ctx.world->get<Health>(*target); h && h->isDead())
                ctx.world->destroy(*target);

            return true;   // ход потрачен (атака)
        }

        // Пусто/стена — tryMove сам разберётся.
        return tryMove(*ctx.world, *ctx.map, ctx.player, dx_, dy_);
    }

private:
    int dx_;
    int dy_;
};

// ---- Прямая атака (стрелки) ----
class AttackCommand final : public Command
{
public:
    explicit AttackCommand(int dx, int dy) noexcept : dx_(dx), dy_(dy) {}

    bool execute(GameContext& ctx) override
    {
        if (!ctx.world || !ctx.map) return false;
        if (!ctx.world->isAlive(ctx.player)) return false;

        auto* ppos = ctx.world->get<Position>(ctx.player);
        if (!ppos) return false;

        const int tx = ppos->x + dx_;
        const int ty = ppos->y + dy_;

        auto target = monsterAt(*ctx.world, tx, ty);
        if (!target) return false;   // пусто — ход не потрачен

        const std::string msg = dealDamage(*ctx.world, *target, 3);
        if (ctx.log) ctx.log->push(msg);

        if (auto* h = ctx.world->get<Health>(*target); h && h->isDead())
            ctx.world->destroy(*target);

        return true;
    }

private:
    int dx_;
    int dy_;
};

// ---- Ожидание ----
class WaitCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        if (ctx.log) ctx.log->push("Вы ждёте...");
        return true;
    }
};
