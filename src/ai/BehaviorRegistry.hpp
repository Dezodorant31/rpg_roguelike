#pragma once

#include "ai/AggressiveBehavior.hpp"
#include "ai/FleeBehavior.hpp"
#include "ai/PatrolBehavior.hpp"

#include <memory>
#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// BehaviorRegistry — реестр поведений по строковому id.
//
// Почему реестр, а не switch:
//   - новый behavior = новая запись в реестре (одна строка), а не ветка
//     в switch в EnemyTurnState;
//   - EnemyTurnState не знает о конкретных Behavior — только о реестре;
//   - тесты могут подсунуть mock-реестр.
//
// Владение: unique_ptr<Behavior>. Реестр — единственный владелец.
// Behavior* живут столько же, сколько реестр.
// ---------------------------------------------------------------------------
class BehaviorRegistry
{
public:
    BehaviorRegistry()
    {
        registerBehavior("aggressive", std::make_unique<AggressiveBehavior>());
        registerBehavior("flee",       std::make_unique<FleeBehavior>());
        registerBehavior("patrol",     std::make_unique<PatrolBehavior>());
    }

    void registerBehavior(const std::string& id,
                          std::unique_ptr<Behavior> behavior)
    {
        behaviors_[id] = std::move(behavior);
    }

    [[nodiscard]] const Behavior* find(const std::string& id) const
    {
        auto it = behaviors_.find(id);
        return it == behaviors_.end() ? nullptr : it->second.get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Behavior>> behaviors_;
};
