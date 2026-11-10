#pragma once

#include <cstdint>
#include <functional>

// ---------------------------------------------------------------------------
// EntityId — идентификатор сущности.
//
// Почему обёртка, а не голый uint32_t:
//   - типобезопасность (нельзя передать случайный int)
//   - хэшируемость «из коробки» (std::hash специализируем)
//   - будущее расширение (сейчас id+generation, потом может стать больше)
//
// generation — «поколение». Когда сущность удаляется, её index попадает в
// пул свободных, а generation увеличивается. Новая сущность получит тот же
// index, но новую generation. Старый EntityId{index, old_gen} перестанет
// быть валидным — это защита от «dangling IDs».
// ---------------------------------------------------------------------------
struct EntityId
{
    std::uint32_t index      = 0;
    std::uint32_t generation = 0;

    [[nodiscard]] bool operator==(const EntityId&) const noexcept = default;
};

// Специализация std::hash — чтобы EntityId можно было использовать
// как ключ в std::unordered_map.
template<>
struct std::hash<EntityId>
{
    [[nodiscard]] std::size_t operator()(const EntityId& e) const noexcept
    {
        // Комбинируем два поля. 0x9e3779b97f4a7c15 — golden ratio constant,
        // классический трюк для смешивания битов.
        std::size_t h = e.index;
        h ^= static_cast<std::size_t>(e.generation) + 0x9e3779b97f4a7c15ULL
             + (h << 6) + (h >> 2);
        return h;
    }
};
