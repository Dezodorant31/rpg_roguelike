#pragma once

#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "world/Map.hpp"

#include <cstdlib>
#include <print>

struct GameContext
{
    World*   world  = nullptr;
    Map*     map    = nullptr;
    EntityId player;
};

class Command
{
public:
    virtual ~Command() = default;
    virtual bool execute(GameContext& ctx) = 0;
};

// ---- Движение ----
class MoveCommand final : public Command
{
public:
    explicit MoveCommand(int dx, int dy) noexcept : dx_(dx), dy_(dy) {}

    bool execute(GameContext& ctx) override
    {
        if (!ctx.world || !ctx.map) return false;
        if (!ctx.world->isAlive(ctx.player)) return false;

        return tryMove(*ctx.world, *ctx.map, ctx.player, dx_, dy_);
    }

private:
    int dx_;
    int dy_;
};

// ---- Атака ----
// Бьёт первого монстра в соседней клетке по (dx, dy).
// Если там стена или пусто — возвращает false (ход не потрачен).
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

        // Ищем монстра в целевой клетке.
        for (auto [id, mpos] : ctx.world->view<Position>())
        {
            if (id == ctx.player) continue;
            if (!ctx.world->get<MonsterTag>(id)) continue;
            if (mpos->x != tx || mpos->y != ty) continue;

            // Нашли. Бьём.
            std::println("{}", dealDamage(*ctx.world, id, 3));
            if (auto* h = ctx.world->get<Health>(id); h && h->isDead())
            {
                std::println("  ...побеждён!");
                ctx.world->destroy(id);
            }
            return true;   // ход потрачен
        }

        // Никого нет — атака «в пустоту». Ход НЕ потрачен.
        return false;
    }

private:
    int dx_;
    int dy_;
};

// ---- Ожидание ----
class WaitCommand final : public Command
{
public:
    bool execute(GameContext&) override { return true; }
};
