#pragma once

#include "ai/BehaviorRegistry.hpp"
#include "states/GameState.hpp"
#include "systems/EnemyAI.hpp"

#include <cassert>
#include <vector>

class EnemyTurnState final : public GameState
{
public:
    explicit EnemyTurnState(const BehaviorRegistry& behaviors,
                            GameState* next = nullptr)
        : behaviors_(behaviors), next_(next) {}

    void setNext(GameState* next) noexcept { next_ = next; }

    bool handleCommand(Command&, GameContext&) override
    {
        return false;
    }

    GameState* update(GameContext& ctx) override
    {
        assert(next_ && "EnemyTurnState::next_ not set");
        if (!ctx.world || !ctx.map || !ctx.log) return next_;

        std::vector<EntityId> monsters;
        for (auto [id, tag] : ctx.world->view<MonsterTag>())
            monsters.push_back(id);

        for (EntityId m : monsters)
        {
            if (!ctx.world->isAlive(m)) continue;
            enemyTakeTurn(*ctx.world, *ctx.map, m, ctx.player, behaviors_, *ctx.log);

            if (!ctx.world->isAlive(ctx.player))
                break;
        }
        return next_;
    }

    [[nodiscard]] const char* name() const override { return "EnemyTurn"; }

private:
    const BehaviorRegistry& behaviors_;
    GameState* next_ = nullptr;
};
