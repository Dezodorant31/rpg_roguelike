#pragma once

#include "ecs/Entity.hpp"

#include <cassert>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

// ---------------------------------------------------------------------------
// World — контейнер всех сущностей и компонентов.
//
// API:
//   create()                    → новая EntityId
//   destroy(id)                 → удалить сущность и все её компоненты
//   isAlive(id)                 → проверить валидность
//   add<T>(id, args...)         → добавить компонент
//   get<T>(id)                  → получить компонент (nullptr если нет)
//   remove<T>(id)               → удалить компонент
//   view<T>()                   → все пары (EntityId, T&) — для систем
//
// Внутри — по одному unordered_map<EntityId, T> на каждый тип компонента.
// Просто, понятно. Оптимизируется позже заменой ComponentStorage<T>.
// ---------------------------------------------------------------------------
class World
{
public:
    // ---- Сущности ----

    [[nodiscard]] EntityId create()
    {
        std::uint32_t index;
        std::uint32_t generation;

        if (!freeIndices_.empty())
        {
            index = freeIndices_.back();
            freeIndices_.pop_back();
            generation = generations_[index];  // текущее поколение слота
        }
        else
        {
            index = static_cast<std::uint32_t>(generations_.size());
            generation = 0;
            generations_.push_back(0);
        }
        return EntityId{index, generation};
    }

    void destroy(EntityId id)
    {
        if (!isAlive(id)) return;

        // Удаляем все компоненты этой сущности.
        for (auto& [type, storage] : storages_)
            storage->erase(id);

        // Помечаем слот как свободный, увеличиваем поколение.
        ++generations_[id.index];
        freeIndices_.push_back(id.index);
    }

    [[nodiscard]] bool isAlive(EntityId id) const noexcept
    {
        return id.index < generations_.size()
            && generations_[id.index] == id.generation;
    }

    // ---- Компоненты ----

    template<typename T, typename... Args>
    T& add(EntityId id, Args&&... args)
    {
        assert(isAlive(id) && "add<T> на мёртвую сущность");
        auto* storage = ensureStorage<T>();
        return storage->emplace(id, std::forward<Args>(args)...);
    }

    template<typename T>
    [[nodiscard]] T* get(EntityId id)
    {
        auto* storage = findStorage<T>();
        return storage ? storage->find(id) : nullptr;
    }

    template<typename T>
    void remove(EntityId id)
    {
        if (auto* storage = findStorage<T>())
            storage->erase(id);
    }

    // ---- View — для систем ----

    template<typename T>
    [[nodiscard]] auto view()
    {
        auto* storage = findStorage<T>();
        if (!storage) return std::vector<std::pair<EntityId, T*>>{};
        return storage->pairs();
    }

private:
    // ---- Type-erased storage ----
    // Один общий интерфейс, чтобы std::unordered_map<type_index, IStorage*>
    // мог держать разнотипные хранилища.
    struct IStorage
    {
        virtual ~IStorage() = default;
        virtual void erase(EntityId id) = 0;
    };

    template<typename T>
    struct Storage final : IStorage
    {
        std::unordered_map<EntityId, T> data;

        template<typename... Args>
        T& emplace(EntityId id, Args&&... args)
        {
            auto [it, _] = data.try_emplace(id, std::forward<Args>(args)...);
            return it->second;
        }

        [[nodiscard]] T* find(EntityId id)
        {
            auto it = data.find(id);
            return it == data.end() ? nullptr : &it->second;
        }

        void erase(EntityId id) override { data.erase(id); }

        [[nodiscard]] std::vector<std::pair<EntityId, T*>> pairs()
        {
            std::vector<std::pair<EntityId, T*>> out;
            out.reserve(data.size());
            for (auto& [id, comp] : data)
                out.emplace_back(id, &comp);
            return out;
        }
    };

    template<typename T>
    Storage<T>* findStorage()
    {
        auto it = storages_.find(std::type_index(typeid(T)));
        if (it == storages_.end()) return nullptr;
        return static_cast<Storage<T>*>(it->second.get());
    }

    template<typename T>
    Storage<T>* ensureStorage()
    {
        auto key = std::type_index(typeid(T));
        auto it = storages_.find(key);
        if (it != storages_.end())
            return static_cast<Storage<T>*>(it->second.get());

        auto storage = std::make_unique<Storage<T>>();
        auto* raw = storage.get();
        storages_.emplace(key, std::move(storage));
        return raw;
    }

    std::vector<std::uint32_t> generations_;
    std::vector<std::uint32_t> freeIndices_;
    std::unordered_map<std::type_index, std::unique_ptr<IStorage>> storages_;
};
