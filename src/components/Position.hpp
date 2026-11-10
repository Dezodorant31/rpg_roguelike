#pragma once

#include <cstdint>

// Позиция в ТАЙЛАХ (не пикселях). Целые числа — у нас сетка.
struct Position
{
    std::int32_t x = 0;
    std::int32_t y = 0;
};
