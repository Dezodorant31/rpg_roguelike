#pragma once

#include "world/Rect.hpp"

#include <string>
#include <random> 

// Комната — прямоугольник + метаданные.
// Пока минимум: id, имя, тип.
struct Room
{
    enum class Type : std::uint8_t
    {
        Normal,      // обычная
        Treasure,    // сокровищница
        Boss,        // босс
        Entrance,    // стартовая (где игрок)
    };

    Rect bounds;
    Type type = Type::Normal;
    int id = 0;

    [[nodiscard]] int centerX() const noexcept { return bounds.centerX(); }
    [[nodiscard]] int centerY() const noexcept { return bounds.centerY(); }

	template<typename RNG>
	bool randomPoint(RNG& rng, int& outX, int& outY) const noexcept
	{
		if (bounds.w <= 2 || bounds.h <= 2) return false;
		std::uniform_int_distribution<int> dx(bounds.x + 1, bounds.right() - 2);
		std::uniform_int_distribution<int> dy(bounds.y + 1, bounds.bottom() - 2);
		outX = dx(rng);
		outY = dy(rng);
		return true;
	}
};
