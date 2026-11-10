#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ecs/World.hpp"

struct Pos { int x, y; };
struct Hp  { int v; };

TEST_CASE("World: создание и удаление")
{
    World w;
    auto a = w.create();
    CHECK(w.isAlive(a));

    w.destroy(a);
    CHECK_FALSE(w.isAlive(a));
}

TEST_CASE("World: generation защищает от повторного использования")
{
    World w;
    auto a = w.create();
    w.destroy(a);
    auto b = w.create();

    CHECK(a.index == b.index);            // тот же слот
    CHECK(a.generation != b.generation);  // но другое поколение
    CHECK_FALSE(w.isAlive(a));
    CHECK(w.isAlive(b));
}

TEST_CASE("World: компоненты")
{
    World w;
    auto e = w.create();

    w.add<Pos>(e, Pos{3, 4});
    CHECK(w.get<Pos>(e)->x == 3);
    CHECK(w.get<Hp>(e) == nullptr);

    w.add<Hp>(e, Hp{10});
    CHECK(w.get<Hp>(e)->v == 10);

    w.remove<Hp>(e);
    CHECK(w.get<Hp>(e) == nullptr);
}

TEST_CASE("World: view")
{
    World w;
    auto a = w.create(); w.add<Pos>(a, Pos{1, 2});
    auto b = w.create(); w.add<Pos>(b, Pos{3, 4});
    auto c = w.create();  // без Pos

    int count = 0;
    for (auto [id, p] : w.view<Pos>()) ++count;
    CHECK(count == 2);
}
