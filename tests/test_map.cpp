#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "world/Map.hpp"

TEST_CASE("Map: размер и границы")
{
    Map m(10, 5);

    CHECK(m.width()  == 10);
    CHECK(m.height() == 5);

    CHECK(m.inBounds(0, 0));
    CHECK(m.inBounds(9, 4));
    CHECK_FALSE(m.inBounds(-1, 0));
    CHECK_FALSE(m.inBounds(0, -1));
    CHECK_FALSE(m.inBounds(10, 0));
    CHECK_FALSE(m.inBounds(0, 5));
}

TEST_CASE("Map: доступ по индексу")
{
    Map m(4, 3);
    m.fill(TileType::Floor);

    m.at(2, 1).type = TileType::Wall;

    CHECK(m.at(2, 1).type == TileType::Wall);
    CHECK(m.at(0, 0).type == TileType::Floor);
    CHECK(m.at(3, 2).type == TileType::Floor);
}

TEST_CASE("Map: tryAt безопасен")
{
    Map m(3, 3);
    Tile out;

    CHECK(m.tryAt(1, 1, out));
    CHECK_FALSE(m.tryAt(-1, 0, out));
    CHECK_FALSE(m.tryAt(3, 0, out));
    CHECK_FALSE(m.tryAt(0, 3, out));
}

TEST_CASE("Map: isWalkable")
{
    Map m(5, 5);
    m.fill(TileType::Wall);

    m.at(2, 2).type = TileType::Floor;

    CHECK(m.isWalkable(2, 2));
    CHECK_FALSE(m.isWalkable(0, 0));
    CHECK_FALSE(m.isWalkable(-1, 0));   // за границей
    CHECK_FALSE(m.isWalkable(10, 0));   // за границей
}
