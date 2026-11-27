#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "ai/AggressiveBehavior.hpp"
#include "ai/FleeBehavior.hpp"
#include "components/Health.hpp"
#include "components/MonsterTag.hpp"
#include "components/Position.hpp"
#include "data/MonsterType.hpp"
#include "ecs/World.hpp"
#include "systems/MessageLog.hpp"
#include "world/Map.hpp"

namespace
{
    EntityId makeMonster(World& w, int x, int y)
    {
        auto id = w.create();
        w.add<MonsterTag>(id);
        w.add<Position>(id, Position{x, y});
        w.add<Health>(id, Health{5, 5});
        return id;
    }

    EntityId makePlayer(World& w, int x, int y)
    {
        auto id = w.create();
        w.add<Position>(id, Position{x, y});
        w.add<Health>(id, Health{30, 30});
        return id;
    }
}

TEST_CASE("Aggressive: идёт к игроку")
{
    World w;
    Map m(20, 20);
    m.fill(TileType::Floor);

    auto monster = makeMonster(w, 5, 5);
    auto player  = makePlayer(w, 10, 5);

    MonsterType type;
    type.id = "test"; type.behaviorId = "aggressive";
    type.damage = 1; type.visionRange = 10;

    AggressiveBehavior behavior;
    MessageLog log;
    behavior.takeTurn(w, m, monster, player, type, log);

    auto* pos = w.get<Position>(monster);
    CHECK(pos->x == 6);   // сдвинулся вправо
    CHECK(pos->y == 5);
}

TEST_CASE("Aggressive: атакует в упор")
{
    World w;
    Map m(20, 20);
    m.fill(TileType::Floor);

    auto monster = makeMonster(w, 5, 5);
    auto player  = makePlayer(w, 6, 5);   // сосед

    MonsterType type;
    type.behaviorId = "aggressive";
    type.damage = 2; type.visionRange = 10;

    AggressiveBehavior behavior;
    MessageLog log;
    behavior.takeTurn(w, m, monster, player, type, log);

    auto* hp = w.get<Health>(player);
    CHECK(hp->current == 28);   // 30 - 2
}

TEST_CASE("Aggressive: не видит за пределами visionRange")
{
    World w;
    Map m(30, 30);
    m.fill(TileType::Floor);

    auto monster = makeMonster(w, 0, 0);
    auto player  = makePlayer(w, 20, 20);   // далеко

    MonsterType type;
    type.behaviorId = "aggressive";
    type.visionRange = 5;

    AggressiveBehavior behavior;
    MessageLog log;
    behavior.takeTurn(w, m, monster, player, type, log);

    auto* pos = w.get<Position>(monster);
    CHECK(pos->x == 0);   // не сдвинулся
    CHECK(pos->y == 0);
}

TEST_CASE("Flee: убегает от игрока")
{
    World w;
    Map m(20, 20);
    m.fill(TileType::Floor);

    auto monster = makeMonster(w, 5, 5);
    auto player  = makePlayer(w, 7, 5);   // справа

    MonsterType type;
    type.behaviorId = "flee";
    type.visionRange = 5;

    FleeBehavior behavior;
    MessageLog log;
    behavior.takeTurn(w, m, monster, player, type, log);

    auto* pos = w.get<Position>(monster);
    CHECK(pos->x == 4);   // сдвинулся ВЛЕВО (от игрока)
    CHECK(pos->y == 5);
}
