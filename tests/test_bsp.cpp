#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "world/BSPDungeonBuilder.hpp"
#include "world/Map.hpp"

#include <queue>
#include <set>

TEST_CASE("BSP: детерминизм по seed")
{
    BSPDungeonBuilder b;
    Dungeon d1 = b.build(42, 60, 40);
    Dungeon d2 = b.build(42, 60, 40);

    REQUIRE(d1.map.width()  == d2.map.width());
    REQUIRE(d1.map.height() == d2.map.height());

    for (int y = 0; y < d1.map.height(); ++y)
        for (int x = 0; x < d1.map.width(); ++x)
            CHECK(d1.map.at(x, y).type == d2.map.at(x, y).type);
}

TEST_CASE("BSP: разные seed — разные карты")
{
    BSPDungeonBuilder b;
    Dungeon d1 = b.build(42, 60, 40);
    Dungeon d2 = b.build(43, 60, 40);

    int diff = 0;
    for (int y = 0; y < d1.map.height(); ++y)
        for (int x = 0; x < d1.map.width(); ++x)
            if (d1.map.at(x, y).type != d2.map.at(x, y).type) ++diff;

    CHECK(diff > 0);
}

TEST_CASE("BSP: есть комнаты и точка спавна проходима")
{
    BSPDungeonBuilder b;
    Dungeon d = b.build(42, 60, 40);

    CHECK(d.rooms.size() >= 2);   // хотя бы две — иначе неинтересно
    CHECK(d.map.inBounds(d.playerSpawnX, d.playerSpawnY));
    CHECK(d.map.isWalkable(d.playerSpawnX, d.playerSpawnY));
}

TEST_CASE("BSP: все комнаты достижимы из точки спавна")
{
    BSPDungeonBuilder b;
    Dungeon d = b.build(42, 60, 40);

    // BFS от спавна
    std::set<std::pair<int,int>> visited;
    std::queue<std::pair<int,int>> q;
    q.push({d.playerSpawnX, d.playerSpawnY});
    visited.insert({d.playerSpawnX, d.playerSpawnY});

    const int dx[] = {0, 0, 1, -1};
    const int dy[] = {1, -1, 0, 0};

    while (!q.empty())
    {
        auto [x, y] = q.front(); q.pop();
        for (int i = 0; i < 4; ++i)
        {
            const int nx = x + dx[i], ny = y + dy[i];
            if (!d.map.inBounds(nx, ny)) continue;
            if (!d.map.isWalkable(nx, ny)) continue;
            if (visited.count({nx, ny})) continue;
            visited.insert({nx, ny});
            q.push({nx, ny});
        }
    }

    // Каждая клетка каждой комнаты должна быть достижима
    for (const auto& room : d.rooms)
    {
        for (int yy = room.bounds.y + 1; yy < room.bounds.bottom() - 1; ++yy)
            for (int xx = room.bounds.x + 1; xx < room.bounds.right() - 1; ++xx)
                CHECK(visited.count({xx, yy}) == 1);
    }
}
