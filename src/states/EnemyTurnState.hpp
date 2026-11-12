#pragma once

#include "states/GameState.hpp"
#include "systems/EnemyAI.hpp"

#include <cassert>
#include <vector>

class EnemyTurnState final : public GameState
{
public:
    explicit EnemyTurnState(GameState* next = nullptr) : next_(next) {}

    void setNext(GameState* next) noexcept { next_ = next; }

    bool handleCommand(Command&, GameContext&) override
    {
        return false;   // монстры не принимают ввод
    }

    GameState* update(GameContext& ctx) override
    {
        assert(next_ && "EnemyTurnState::next_ not set");
        if (!ctx.world || !ctx.map || !ctx.log) return next_;

        // Снимок монстров — чтобы мутации не сломали итерацию
        std::vector<EntityId> monsters;
        for (auto [id, tag] : ctx.world->view<MonsterTag>())
            monsters.push_back(id);

        for (EntityId m : monsters)
        {
            if (!ctx.world->isAlive(m)) continue;   // мог умереть
            enemyTakeTurn(*ctx.world, *ctx.map, m, ctx.player, *ctx.log);

            if (!ctx.world->isAlive(ctx.player))
                break;   // игрок мёртв — дальше не ходим
        }
        return next_;
    }

    [[nodiscard]] const char* name() const override { return "EnemyTurn"; }

private:
    GameState* next_ = nullptr;
};
