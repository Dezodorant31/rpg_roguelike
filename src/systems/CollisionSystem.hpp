#pragma once

#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"

// Занята ли клетка (x, y) какой-либо сущностью?
// «Сущность» = у кого есть Position. Игрок и монстры — да.
// Предметы — тоже будут (но тогда добавим флаг «блокирует ли»).
//
// ignore — ID, которого игнорируем (обычно сам идущий).
[[nodiscard]] inline bool isOccupied(World& world,
                                     int x, int y,
                                     EntityId ignore = EntityId{})
{
    for (auto [id, pos] : world.view<Position>())
    {
        if (id == ignore) continue;
        if (pos->x == x && pos->y == y) return true;
    }
    return false;
}

// Найти сущность в клетке (x, y). Возвращает «мёртвый» EntityId,
// если никого нет (generation == 0 и index == 0 — заведомо невалидный,
// потому что index 0 — это первая созданная сущность, у неё generation 0,
// но isAlive вернёт true. Поэтому используем «пустой» id и проверяем через
// isAlive).
//
// ВАЖНО: возвращать «мёртвый» ID — это нормально, isAlive() его отсеет.
// Но чтобы не путать, лучше возвращать std::optional<EntityId>.
[[nodiscard]] inline std::optional<EntityId> entityAt(World& world, int x, int y)
{
    for (auto [id, pos] : world.view<Position>())
    {
        if (pos->x == x && pos->y == y) return id;
    }
    return std::nullopt;
}

// Найти МОНСТРА в клетке (x, y).
[[nodiscard]] inline std::optional<EntityId> monsterAt(World& world, int x, int y)
{
    for (auto [id, pos] : world.view<Position>())
    {
        if (pos->x != x || pos->y != y) continue;
        if (world.get<MonsterTag>(id)) return id;
    }
    return std::nullopt;
}
