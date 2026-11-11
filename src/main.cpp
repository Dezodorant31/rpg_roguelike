#include <SFML/Graphics.hpp>
#include <print>
#include <algorithm>

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
#include "systems/RenderSystem.hpp"
#include "world/Map.hpp"

int main()
{
    constexpr int kTileSize  = 32;
    constexpr int kMapWidth  = 40;
    constexpr int kMapHeight = 25;

    sf::RenderWindow window(
        sf::VideoMode({1280u, 720u}),
        "rpg_roguelike",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // ---------- Мир ----------
    Map map(kMapWidth, kMapHeight);
    map.fill(TileType::Floor);

    for (int x = 0; x < kMapWidth; ++x) {
        map.at(x, 0).type = TileType::Wall;
        map.at(x, kMapHeight - 1).type = TileType::Wall;
    }
    for (int y = 0; y < kMapHeight; ++y) {
        map.at(0, y).type = TileType::Wall;
        map.at(kMapWidth - 1, y).type = TileType::Wall;
    }
    for (int y = 5; y < 15; ++y)
        map.at(10, y).type = TileType::Wall;

    for (auto& t : map.tiles())
        t.color = (t.type == TileType::Floor)
                ? sf::Color(60, 60, 70) : sf::Color(30, 30, 35);

    // ---------- ECS ----------
    World world;

    const EntityId player = world.create();
    world.add<PlayerTag>(player);
    world.add<Name>(player, "Вы");
    world.add<Position>(player, Position{3, 3});
    world.add<Renderable>(player, Renderable{sf::Color::Green});
    world.add<Health>(player, Health{30, 30});

    const EntityId goblin = world.create();
    world.add<MonsterTag>(goblin);
    world.add<Name>(goblin, "Гоблин");
    world.add<Position>(goblin, Position{7, 4});
    world.add<Renderable>(goblin, Renderable{sf::Color(180, 60, 60)});
    world.add<Health>(goblin, Health{5, 5});

    const EntityId rat = world.create();
    world.add<MonsterTag>(rat);
    world.add<Name>(rat, "Крыса");
    world.add<Position>(rat, Position{14, 8});
    world.add<Renderable>(rat, Renderable{sf::Color(140, 100, 60), 0.6f});
    world.add<Health>(rat, Health{3, 3});

    // ---------- Состояния ----------
    // Порядок: GameOver ← EnemyTurn ← PlayerTurn
    // Связи устанавливаем после создания всех объектов.
    GameOverState  gameOver;
    EnemyTurnState enemyTurn{ &gameOver };       // после монстров → GameOver (main проверит)
    PlayerTurnState playerTurn{ &enemyTurn };    // после игрока → EnemyTurn

    GameState* current = &playerTurn;

    // ---------- Контекст и системы ----------
    GameContext ctx{ &world, &map, player };
    InputHandler input;
    RenderSystem renderer(kTileSize);
    GameLoop loop;

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

            // Игрок мёртв? Переключаемся на GameOver.
            if (auto* hp = world.get<Health>(player); hp && hp->isDead())
            {
                current = &gameOver;
            }

            // Создать команду по клавише (если маппинг есть)
            auto cmd = input.makeCommand(*event);
            if (!cmd) continue;

            // Отдать команду текущему состоянию
            current->handleCommand(*cmd, ctx);
        }

        // Игрок мёртв — принудительно GameOver
        if (auto* hp = world.get<Health>(player); hp && hp->isDead())
            current = &gameOver;

        // Симуляция
        const int steps = loop.advance();
        for (int i = 0; i < steps; ++i) { /* анимации — потом */ }

        // Обновить состояние (возможно, перейти дальше)
        if (GameState* next = current->update(ctx); next != nullptr && next != current)
            current = next;

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
        renderer.drawMap(window, map, camX, camY);
        renderer.drawEntities(window, world, camX, camY);
        window.display();
    }

    std::println("Пока!");
    return 0;
}
