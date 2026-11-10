#pragma once

#include "components/Health.hpp"
#include "components/Name.hpp"
#include "ecs/World.hpp"

#include <string>

// Наносит урон. Возвращает строку для лога: «Гоблин получает 3 урона (2/5)».
inline std::string dealDamage(World& world, EntityId target, int amount)
{
    auto* hp = world.get<Health>(target);
    if (!hp) return {};

    hp->current -= amount;

    std::string name = "Сущность";
    if (auto* n = world.get<Name>(target))
        name = n->value;

    return name + " получает " + std::to_string(amount) + " урона ("
         + std::to_string(hp->current) + "/" + std::to_string(hp->max) + ")";
}
