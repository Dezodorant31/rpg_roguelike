#pragma once

#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/MovementSystem.hpp"
#include "world/Map.hpp"

struct GameContext
{
    World* world = nullptr;
    Map*   map   = nullptr;
    EntityId player;   // может быть «мёртвым», проверяем через world->isAlive
};

class Command
{
public:
    virtual ~Command() = default;
    virtual bool execute(GameContext& ctx) = 0;
};

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

class WaitCommand final : public Command
{
public:
    bool execute(GameContext&) override { return true; }
};
