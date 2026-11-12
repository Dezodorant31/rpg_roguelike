#pragma once

#include "world/Map.hpp"
#include "world/Room.hpp"

#include <cstdint>
#include <vector>

// Результат работы DungeonBuilder.
// Содержит готовую карту + метаданные (комнаты, seed, точка спавна игрока).
struct Dungeon
{
    Map map{1, 1};                // будет перезаписан билдером
    std::vector<Room> rooms;
    std::uint32_t seed = 0;

    // Где спавнить игрока (центр первой комнаты)
    int playerSpawnX = 0;
    int playerSpawnY = 0;

    [[nodiscard]] bool isValid() const noexcept
    {
        return !rooms.empty()
            && map.inBounds(playerSpawnX, playerSpawnY)
            && map.isWalkable(playerSpawnX, playerSpawnY);
    }
};
