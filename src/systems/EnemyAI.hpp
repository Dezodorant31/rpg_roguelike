#pragma once

#include "ai/Behavior.hpp"
#include "ai/BehaviorRegistry.hpp"
#include "components/MonsterTypeRef.hpp"
#include "components/Position.hpp"
#include "ecs/World.hpp"
#include "systems/MessageLog.hpp"
#include "world/Map.hpp"

// ---------------------------------------------------------------------------
// enemyTakeTurn — один ход монстра.
//
// Теперь: не «преследуй игрока», а «найди тип монстра → найди его
// Behavior в реестре → вызови takeTurn».
//
// Порядок поиска:
//   1. MonsterTypeRef — есть ли у монстра тип? Если нет — пропускаем
//      (это «безымянный» монстр, поведение по умолчанию — стоять).
//   2. BehaviorRegistry::find(type->behaviorId) — есть ли такое поведение?
//      Если нет — пропускаем (опечатка в JSON, неизвестный behavior).
//
// Почему не assert на отсутствие: в проде JSON может быть кривой, и
// лучше «монстр стоит», чем «игра падает». Ошибку загрузки ловит
// MonsterTypeRegistry в конструкторе (исключение), но behaviorId —
// строка, её валидность проверяется здесь.
// ---------------------------------------------------------------------------
inline void enemyTakeTurn(World& world, Map& map,
                          EntityId monster, EntityId player,
                          const BehaviorRegistry& behaviors,
                          MessageLog& log)
{
    if (!world.isAlive(monster) || !world.isAlive(player)) return;

    auto* ref = world.get<MonsterTypeRef>(monster);
    if (!ref || !ref->type) return;

    const Behavior* behavior = behaviors.find(ref->type->behaviorId);
    if (!behavior) return;

    behavior->takeTurn(world, map, monster, player, *ref->type, log);
}
