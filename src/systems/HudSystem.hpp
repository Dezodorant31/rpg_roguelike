#pragma once

#include "components/Health.hpp"
#include "components/Name.hpp"
#include "ecs/World.hpp"
#include "systems/MessageLog.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include <algorithm>
#include <string>

class HudSystem
{
public:
    explicit HudSystem(const sf::Font& font) : font_(font) {}

    void draw(sf::RenderWindow& window,
              World& world,
              EntityId player,
              const MessageLog& log) const
    {
        drawHealthBar(window, world, player, {8.f, 8.f});

        const auto winSize = window.getSize();
        const float logHeight = kLineHeight * static_cast<float>(log.capacity());
        drawLog(window, log, {8.f, static_cast<float>(winSize.y) - 8.f - logHeight});
    }

private:
    static constexpr float kLineHeight = 18.f;
    static constexpr float kBarWidth   = 200.f;
    static constexpr float kBarHeight  = 20.f;

    void drawHealthBar(sf::RenderWindow& window, World& world,
                       EntityId player, sf::Vector2f pos) const
    {
        if (!world.isAlive(player)) return;
        auto* hp = world.get<Health>(player);
        if (!hp) return;

        const float ratio = hp->max > 0
            ? static_cast<float>(hp->current) / static_cast<float>(hp->max)
            : 0.f;

        sf::RectangleShape bg({kBarWidth, kBarHeight});
        bg.setPosition(pos);
        bg.setFillColor(sf::Color(40, 20, 20));
        window.draw(bg);

        sf::RectangleShape fill({
            kBarWidth * std::clamp(ratio, 0.f, 1.f), kBarHeight});
        fill.setPosition(pos);
        fill.setFillColor(ratio > 0.5f ? sf::Color(60, 180, 60)
                       : ratio > 0.25f ? sf::Color(200, 160, 40)
                                       : sf::Color(200, 60, 60));
        window.draw(fill);

        sf::RectangleShape border({kBarWidth, kBarHeight});
        border.setPosition(pos);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color(120, 120, 140));
        border.setOutlineThickness(1.f);
        window.draw(border);

        std::string label = "HP " + std::to_string(hp->current)
                  + "/" + std::to_string(hp->max);
		if (auto* n = world.get<Name>(player); n)
			label = n->value + ": " + label;

		sf::Text text(font_, sf::String::fromUtf8(label.begin(), label.end()), 14);
        text.setFillColor(sf::Color::White);
        text.setPosition({pos.x + 6.f, pos.y + 2.f});
        window.draw(text);
    }

    void drawLog(sf::RenderWindow& window, const MessageLog& log,
             sf::Vector2f pos) const
	{
		float y = pos.y;
		for (const auto& line : log.lines())          // ← line, не label
		{
			sf::Text text(font_, sf::String::fromUtf8(line.begin(), line.end()), 14);  // ← line
			text.setFillColor(sf::Color(200, 200, 200));
			text.setPosition({pos.x, y});
			window.draw(text);
			y += kLineHeight;
		}
	}

    const sf::Font& font_;
};
