#include <SFML/Graphics.hpp>
#include <print>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1280u, 720u}),
        "rpg_roguelike — SFML 3.1",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    // Простейший шрифт-агностичный прямоугольник, чтобы видеть, что рендер жив
    sf::RectangleShape tile({32.f, 32.f});
    tile.setFillColor(sf::Color::Green);
    tile.setPosition({100.f, 100.f});

    while (window.isOpen())
    {
        // SFML 3: обработка событий через std::optional<sf::Event>
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        window.clear(sf::Color(20, 20, 28));
        window.draw(tile);
        window.display();
    }

    std::println("Пока!");
    return 0;
}
