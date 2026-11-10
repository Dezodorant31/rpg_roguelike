#include <SFML/Graphics.hpp>
#include <print>

#include "input/InputHandler.hpp"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1280u, 720u}),
        "rpg_roguelike — SFML 3.1",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    sf::RectangleShape player({32.f, 32.f});
    player.setFillColor(sf::Color::Green);
    player.setPosition({100.f, 100.f});

    GameContext ctx{ &player };

    InputHandler input;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                continue;
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>();
                key && key->code == sf::Keyboard::Key::Escape)
            {
                window.close();
                continue;
            }

            // Ввод через команды
            input.handleEvent(*event, ctx);
        }

        window.clear(sf::Color(20, 20, 28));
        window.draw(player);
        window.display();
    }

    std::println("Пока!");
    return 0;
}
