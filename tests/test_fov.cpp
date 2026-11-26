#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "systems/FOVSystem.hpp"
#include "world/Map.hpp"

TEST_CASE("FOV: в открытой комнате видно всё в радиусе")
{
    Map m(10, 10);
    m.fill(TileType::Floor);

    FOVSystem::compute(m, 5, 5, 3);

    CHECK(m.at(5, 5).visible);
    CHECK(m.at(5, 4).visible);
    CHECK(m.at(6, 5).visible);
    CHECK(m.at(4, 5).visible);
    CHECK(m.at(5, 6).visible);
    CHECK(m.at(8, 5).visible);
    CHECK(m.at(2, 5).visible);
}

TEST_CASE("FOV: за стеной не видно")
{
    Map m(20, 5);
    m.fill(TileType::Floor);

    for (int y = 0; y < 5; ++y)
        m.at(10, y).type = TileType::Wall;

    FOVSystem::compute(m, 5, 2, 10);

    CHECK(m.at(10, 2).visible);          // стена видна
    CHECK_FALSE(m.at(11, 2).visible);    // за стеной — нет
    CHECK_FALSE(m.at(15, 2).visible);
}

TEST_CASE("FOV: explored накапливается")
{
    Map m(10, 10);
    m.fill(TileType::Floor);

    FOVSystem::compute(m, 2, 2, 2);
    CHECK(m.at(2, 2).explored);

    FOVSystem::compute(m, 8, 8, 2);

    CHECK_FALSE(m.at(2, 2).visible);
    CHECK(m.at(2, 2).explored);          // помним
    CHECK(m.at(8, 8).visible);
    CHECK(m.at(8, 8).explored);
}

TEST_CASE("FOV: за границей карты не падаем")
{
    Map m(5, 5);
    m.fill(TileType::Floor);

    FOVSystem::compute(m, 0, 0, 10);

    CHECK(m.at(0, 0).visible);
    CHECK(m.at(4, 4).visible);
}
