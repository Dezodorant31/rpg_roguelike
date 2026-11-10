#include "world/Map.hpp"

#include <cassert>

Map::Map(int width, int height)
    : width_(width)
    , height_(height)
    , tiles_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), makeWall())
{
    assert(width > 0 && height > 0 && "Map must have positive size");
}

void Map::fill(TileType type)
{
    const Tile t = (type == TileType::Floor) ? makeFloor() : makeWall();
    for (auto& tile : tiles_)
        tile = t;
}
