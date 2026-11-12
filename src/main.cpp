#include <SFML/Graphics.hpp>
#include <print>
#include <algorithm>
#include <random>

#include "components/Health.hpp"
#include "components/MonsterTag.hpp"
#include "components/Name.hpp"
#include "components/PlayerTag.hpp"
#include "components/Position.hpp"
#include "components/Renderable.hpp"
#include "core/GameLoop.hpp"
#include "ecs/World.hpp"
#include "input/InputHandler.hpp"
#include "states/EnemyTurnState.hpp"
#include "states/GameOverState.hpp"
#include "states/PlayerTurnState.hpp"
#include "systems/HudSystem.hpp"
#include "systems/MessageLog.hpp"
#include "systems/RenderSystem.hpp"
#include "world/BSPDungeonBuilder.hpp"
#include "world/DungeonDirector.hpp"
#include "world/Map.hpp"

// Спавн монстров в случайных комнатах
static void spawnMonsters(World& world, const Dungeon& dungeon,
                          std::mt19937& rng)
{
    if (dungeon.rooms.size() < 2) return;

    // Пропускаем первую (Entrance)
    std::uniform_int_distribution<std::size_t> roomDist(1, dungeon.rooms.size() - 1);
    std::uniform_int_distribution<int> dx(1, 2);   // 1–2 гоблина
    std::uniform_int_distribution<int> dy(0, 1);   // 0–1 крыса

    const int numGoblins = dx(rng);
    const int numRats    = dy(rng);

    auto spawnAt = [&](const Room& room, sf::Color color, float sizeRatio,
                       const std::string& name, int hp)
    {
        int x, y;
        if (!room.randomPoint(rng, x, y)) return;

        const EntityId id = world.create();
        world.add<MonsterTag>(id);
        world.add<Name>(id, name);
        world.add<Position>(id, Position{x, y});
        world.add<Renderable>(id, Renderable{color, sizeRatio});
        world.add<Health>(id, Health{hp, hp});
    };

    for (int i = 0; i < numGoblins; ++i)
    {
        const auto& room = dungeon.rooms[roomDist(rng)];
        spawnAt(room, sf::Color(180, 60, 60), 0.875f, "Гоблин", 5);
    }
    for (int i = 0; i < numRats; ++i)
    {
        const auto& room = dungeon.rooms[roomDist(rng)];
        spawnAt(room, sf::Color(140, 100, 60), 0.6f, "Крыса", 3);
    }
}

int main()
{
    constexpr int kTileSize  = 32;
    constexpr int kMapWidth  = 60;
    constexpr int kMapHeight = 40;

    sf::RenderWindow window(
        sf::VideoMode({1280u, 720u}),
        "rpg_roguelike",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // ---------- Шрифт ----------
    sf::Font font;
    if (!font.openFromFile("assets/fonts/main.ttf"))
    {
        std::println(stderr, "Не удалось загрузить шрифт: assets/fonts/main.ttf");
        return 1;
    }

    // ---------- Генерация подземелья ----------
    const std::uint32_t seed = static_cast<std::uint32_t>(std::random_device{}());
    std::println("Seed: {}", seed);

    DungeonDirector director;
    director.registerBuilder("bsp", std::make_unique<BSPDungeonBuilder>());

    auto dungeonOpt = director.build("bsp", seed, kMapWidth, kMapHeight);
    if (!dungeonOpt)
    {
        std::println(stderr, "Не удалось сгенерировать подземелье");
        return 1;
    }
    Dungeon dungeon = std::move(*dungeonOpt);

    std::println("Комнат: {}", dungeon.rooms.size());
    std::println("Спавн игрока: ({}, {})",
                 dungeon.playerSpawnX, dungeon.playerSpawnY);

    std::mt19937 rng(seed + 1);   // отдельный RNG для спавна (чтобы не портить карту)

    // ---------- ECS ----------
    World world;

    const EntityId player = world.create();
    world.add<PlayerTag>(player);
    world.add<Name>(player, "Вы");
    world.add<Position>(player, Position{dungeon.playerSpawnX, dungeon.playerSpawnY});
    world.add<Renderable>(player, Renderable{sf::Color::Green});
    world.add<Health>(player, Health{30, 30});

    spawnMonsters(world, dungeon, rng);

    // ---------- Состояния ----------
    GameOverState   gameOver;
    EnemyTurnState  enemyTurn;
    PlayerTurnState playerTurn{ &enemyTurn };
    enemyTurn.setNext(&playerTurn);

    GameState* current = &playerTurn;

    // ---------- Системы и контекст ----------
    MessageLog log;
    GameContext ctx{ &world, &dungeon.map, &log, player };
    InputHandler input;
    RenderSystem renderer(kTileSize);
    HudSystem hud(font);
    GameLoop loop;

    log.push("Добро пожаловать в подземелье!");

    // ---------- Игровой цикл ----------
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) { window.close(); continue; }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>();
                key && key->code == sf::Keyboard::Key::Escape)
            {
                window.close(); continue;
            }

            auto cmd = input.makeCommand(*event);
            if (!cmd) continue;

            current->handleCommand(*cmd, ctx);
        }

        const int steps = loop.advance();
        for (int i = 0; i < steps; ++i) { }

        if (GameState* next = current->update(ctx); next != nullptr && next != current)
            current = next;

        if (auto* hp = world.get<Health>(player); hp && hp->isDead())
        {
            if (current != &gameOver)
            {
                log.push("Вы погибли...");
                current = &gameOver;
            }
        }

        // Камера
        float camX = 0.f, camY = 0.f;
        if (auto* ppos = world.get<Position>(player))
        {
            camX = static_cast<float>(ppos->x * kTileSize)
                 - static_cast<float>(window.getSize().x) / 2.f;
            camY = static_cast<float>(ppos->y * kTileSize)
                 - static_cast<float>(window.getSize().y) / 2.f;
        }
        const float maxCamX = static_cast<float>(kMapWidth  * kTileSize)
                            - static_cast<float>(window.getSize().x);
        const float maxCamY = static_cast<float>(kMapHeight * kTileSize)
                            - static_cast<float>(window.getSize().y);
        camX = std::clamp(camX, 0.f, std::max(0.f, maxCamX));
        camY = std::clamp(camY, 0.f, std::max(0.f, maxCamY));

        // Рендер
        window.clear(sf::Color(10, 10, 15));
        renderer.drawMap(window, dungeon.map, camX, camY);
        renderer.drawEntities(window, world, camX, camY);
        hud.draw(window, world, player, log);
        window.display();
    }

    std::println("Пока!");
    return 0;
}
