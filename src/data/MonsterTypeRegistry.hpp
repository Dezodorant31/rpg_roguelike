#pragma once

#include "data/MonsterType.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// MonsterTypeRegistry — реестр типов монстров, загруженных из JSON.
//
// JSON-формат (assets/data/monsters.json):
// {
//   "monsters": [
//     { "id": "goblin", "name": "Гоблин", "behavior": "aggressive",
//       "hp": 5, "damage": 1, "vision": 8,
//       "color": [180, 60, 60], "size": 0.875 },
//     ...
//   ]
// }
//
// Почему загрузка в конструкторе, а не статически:
//   - путь к JSON — параметр. В тестах можно подсунуть свой файл;
//   - ошибка загрузки — исключение. Это фатально (без типов монстров игра
//     не работает), поэтому исключение уместно, в отличие от игровой логики.
// ---------------------------------------------------------------------------
class MonsterTypeRegistry
{
public:
    explicit MonsterTypeRegistry(const std::string& jsonPath)
    {
        std::ifstream file(jsonPath);
        if (!file.is_open())
            throw std::runtime_error("Не удалось открыть " + jsonPath);

        nlohmann::json j;
        file >> j;

        for (const auto& entry : j.at("monsters"))
        {
            MonsterType t;
            t.id          = entry.at("id").get<std::string>();
            t.name        = entry.at("name").get<std::string>();
            t.behaviorId  = entry.at("behavior").get<std::string>();
            t.hp          = entry.value("hp", 5);
            t.damage      = entry.value("damage", 1);
            t.visionRange = entry.value("vision", 8);

            if (entry.contains("color"))
            {
                const auto& c = entry.at("color");
                t.color = sf::Color(
                    c.at(0).get<std::uint8_t>(),
                    c.at(1).get<std::uint8_t>(),
                    c.at(2).get<std::uint8_t>());
            }
            t.sizeRatio = entry.value("size", 0.875f);

            types_[t.id] = std::move(t);
        }
    }

    [[nodiscard]] const MonsterType* find(const std::string& id) const
    {
        auto it = types_.find(id);
        return it == types_.end() ? nullptr : &it->second;
    }

    [[nodiscard]] const MonsterType& at(const std::string& id) const
    {
        auto* t = find(id);
        if (!t) throw std::out_of_range("MonsterType not found: " + id);
        return *t;
    }

private:
    std::unordered_map<std::string, MonsterType> types_;
};
