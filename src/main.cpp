#include <SFML/Graphics.hpp>
#include <print>
#include <algorithm>   // std::clamp

#include "core/GameLoop.hpp"
#include "input/InputHandler.hpp"
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

    // --- Мир ---
    Map map(kMapWidth, kMapHeight);
    map.fill(TileType::Floor);

    // Окружаем стенами (рамка)
    for (int x = 0; x < kMapWidth; ++x)
    {
        map.at(x, 0).type = TileType::Wall;
        map.at(x, kMapHeight - 1).type = TileType::Wall;
    }
    for (int y = 0; y < kMapHeight; ++y)
    {
        map.at(0, y).type = TileType::Wall;
        map.at(kMapWidth - 1, y).type = TileType::Wall;
    }
    // Внутренняя стена для проверки коллизий
    for (int y = 5; y < 15; ++y)
        map.at(10, y).type = TileType::Wall;

    // Пересобираем цвета (потому что поменяли type напрямую)
    for (auto& t : map.tiles())
    {
        t.color = (t.type == TileType::Floor)
                ? sf::Color(60, 60, 70)
                : sf::Color(30, 30, 35);
    }

    // --- Игрок ---
    int playerX = 3;
    int playerY = 3;

    GameContext ctx{ &map, &playerX, &playerY };
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

            // Команда вернула true → ход потрачен.
            // Пока некому «отвечать» на ход, но семантика уже правильная.
            [[maybe_unused]] bool turnTaken = input.handleEvent(*event, ctx);
        }

        // --- Симуляция ---
        // В пошаговой игре тут пока пусто. GameLoop уже готов
        // для будущих анимаций (интерполяция между шагами).
        const int steps = loop.advance();
        for (int i = 0; i < steps; ++i)
        {
            // будущее: update animations, effects
        }

        // --- Камера центрируется на игроке ---
        float camX = static_cast<float>(playerX * kTileSize)
                   - static_cast<float>(window.getSize().x) / 2.f;
        float camY = static_cast<float>(playerY * kTileSize)
                   - static_cast<float>(window.getSize().y) / 2.f;

        // Клампим камеру, чтобы не показывать пустоту за картой
        const float maxCamX = static_cast<float>(kMapWidth  * kTileSize)
                            - static_cast<float>(window.getSize().x);
        const float maxCamY = static_cast<float>(kMapHeight * kTileSize)
                            - static_cast<float>(window.getSize().y);
        camX = std::clamp(camX, 0.f, std::max(0.f, maxCamX));
        camY = std::clamp(camY, 0.f, std::max(0.f, maxCamY));

        // --- Рендер ---
        window.clear(sf::Color(10, 10, 15));
        renderer.drawMap(window, map, camX, camY);

        // Игрок — отдельный прямоугольник
        sf::RectangleShape player({static_cast<float>(kTileSize) - 4.f,
                                   static_cast<float>(kTileSize) - 4.f});
        player.setFillColor(sf::Color::Green);
        player.setPosition({
            static_cast<float>(playerX * kTileSize) - camX + 2.f,
            static_cast<float>(playerY * kTileSize) - camY + 2.f
        });
        window.draw(player);

        window.display();
    }

    std::println("Пока!");
    return 0;
}
