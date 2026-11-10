#include "Command.hpp"

bool MoveUpCommand::move(GameContext& ctx, sf::Vector2f dir)
{
    if (!ctx.player) return false;
    ctx.player->move({dir.x * ctx.tileSize, dir.y * ctx.tileSize});
    return true;
}

bool MoveDownCommand::move(GameContext& ctx, sf::Vector2f dir)
{
    if (!ctx.player) return false;
    ctx.player->move({dir.x * ctx.tileSize, dir.y * ctx.tileSize});
    return true;
}

bool MoveLeftCommand::move(GameContext& ctx, sf::Vector2f dir)
{
    if (!ctx.player) return false;
    ctx.player->move({dir.x * ctx.tileSize, dir.y * ctx.tileSize});
    return true;
}

bool MoveRightCommand::move(GameContext& ctx, sf::Vector2f dir)
{
    if (!ctx.player) return false;
    ctx.player->move({dir.x * ctx.tileSize, dir.y * ctx.tileSize});
    return true;
}
