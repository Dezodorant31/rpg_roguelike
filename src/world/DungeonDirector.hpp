#pragma once

#include "world/Dungeon.hpp"
#include "world/DungeonBuilder.hpp"

#include <cstdint>
#include <memory>

// Оркестратор. Выбирает билдер и вызывает build.
// Пока один билдер (BSP), но интерфейс готов к нескольким.
//
// GoF Director: «управляет процессом сборки».
class DungeonDirector
{
public:
    void registerBuilder(const std::string& name,
                        std::unique_ptr<DungeonBuilder> builder)
    {
        builders_[name] = std::move(builder);
    }

    // Построить. Если builderName не найден — nullptr.
    [[nodiscard]] std::optional<Dungeon> build(const std::string& builderName,
                                               std::uint32_t seed,
                                               int width, int height)
    {
        auto it = builders_.find(builderName);
        if (it == builders_.end()) return std::nullopt;

        Dungeon d = it->second->build(seed, width, height);
        if (!d.isValid()) return std::nullopt;
        return d;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<DungeonBuilder>> builders_;
};
