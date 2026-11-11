#pragma once

#include "states/GameState.hpp"
#include "systems/EnemyAI.hpp"

#include <vector>

// Все монстры делают по одному действию.
// Как только все сходили (или игрок умер) — переход к next_.
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
        if (!ctx.world || !ctx.map) return next_;

        // Снимок ID монстров — чтобы не инвалидировать view во время итерации.
        std::vector<EntityId> monsters;
        for (auto [id, pos] : ctx.world->view<MonsterTag>())
            monsters.push_back(id);

        for (EntityId m : monsters)
        {
            // Монстр мог умереть от предыдущего действия — проверяем.
            if (!ctx.world->isAlive(m)) continue;
            enemyTakeTurn(*ctx.world, *ctx.map, m, ctx.player);

            // Игрок мог умереть — тогда GameOver.
            // Пока просто выходим, main проверит.
            if (!ctx.world->isAlive(ctx.player))
                return next_;
        }
        return next_;
    }

    [[nodiscard]] const char* name() const override { return "EnemyTurn"; }

private:
    GameState* next_ = nullptr;
};
