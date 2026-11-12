#pragma once

#include "components/Health.hpp"
#include "components/Name.hpp"
#include "ecs/World.hpp"

#include <string>

// Наносит урон. Возвращает строку для лога.
// Формат: «Гоблин получает 3 урона (2/5)» или «Гоблин погибает!».
// Вызывающий решает, куда эту строку девать (MessageLog, консоль, файл).
inline std::string dealDamage(World& world, EntityId target, int amount)
{
    auto* hp = world.get<Health>(target);
    if (!hp) return {};

    hp->current -= amount;

    std::string name = "Сущность";
    if (auto* n = world.get<Name>(target))
        name = n->value;

    if (hp->isDead())
        return name + " погибает!";

    return name + " получает " + std::to_string(amount) + " урона ("
         + std::to_string(hp->current) + "/" + std::to_string(hp->max) + ")";
}
