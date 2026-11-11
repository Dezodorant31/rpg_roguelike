#pragma once

#include "states/GameState.hpp"

// Игра окончена. Ничего не принимает, никуда не переходит.
class GameOverState final : public GameState
{
public:
    bool handleCommand(Command&, GameContext&) override { return false; }

    GameState* update(GameContext&) override { return nullptr; }

    [[nodiscard]] const char* name() const override { return "GameOver"; }
};
