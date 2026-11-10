#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

// Контекст, который видят команды.
// Пока минимален — потом сюда придёт Game/World.
struct GameContext
{
    sf::RectangleShape* player;   // nullptr-safe: команды проверяют
    int tileSize = 32;
};

// Базовый интерфейс команды.
// execute() возвращает bool: "команда что-то сделала" —
// пригодится для пошаговой логики (сделал ход → ходят монстры).
class Command
{
public:
    virtual ~Command() = default;
    virtual bool execute(GameContext& ctx) = 0;
};

// -------------------- Конкретные команды --------------------

class MoveUpCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        return move(ctx, {0.f, -1.f});
    }
private:
    static bool move(GameContext& ctx, sf::Vector2f dir);
};

class MoveDownCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        return move(ctx, {0.f, 1.f});
    }
private:
    static bool move(GameContext& ctx, sf::Vector2f dir);
};

class MoveLeftCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        return move(ctx, {-1.f, 0.f});
    }
private:
    static bool move(GameContext& ctx, sf::Vector2f dir);
};

class MoveRightCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        return move(ctx, {1.f, 0.f});
    }
private:
    static bool move(GameContext& ctx, sf::Vector2f dir);
};

// Пробел — тестовая «команда ожидания».
// Позже станет WaitCommand (пропуск хода) или ActionCommand.
class WaitCommand final : public Command
{
public:
    bool execute(GameContext& ctx) override
    {
        if (!ctx.player) return false;
        // Меняем цвет — визуальный отклик, что команда дошла
        auto c = ctx.player->getFillColor();
        ctx.player->setFillColor(
            c == sf::Color::Green ? sf::Color::Yellow : sf::Color::Green);
        return true;
    }
};
