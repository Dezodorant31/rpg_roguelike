#pragma once

#include <SFML/Graphics/Color.hpp>

// «Как нарисовать эту сущность».
// Сейчас — цвет и размер. Потом добавим sprite_id для анимаций.
struct Renderable
{
    sf::Color color      = sf::Color::White;
    float     sizeRatio  = 0.875f;  // 0.875 * 32 = 28 px (чуть меньше тайла)
};
