#pragma once

#include "world/Dungeon.hpp"

#include <cstdint>

// Интерфейс билдера подземелий.
// Наследники реализуют build().
//
// Почему интерфейс, а не одна функция:
//   - BSPDungeonBuilder, CellularAutomataBuilder, ...
//   - DungeonDirector выбирает билдер по seed/параметрам
//   - тесты могут подсунуть MockDungeonBuilder
class DungeonBuilder
{
public:
    virtual ~DungeonBuilder() = default;

    // Построить подземелье по seed.
    // Детерминированно: один seed → одна карта.
    [[nodiscard]] virtual Dungeon build(std::uint32_t seed,
                                        int width, int height) = 0;
};
