#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>

// ---------------------------------------------------------------------------
// MonsterType — «тип монстра» как данные.
//
// Один экземпляр на тип (гоблин, крыса, орк). Все гоблины в мире ссылаются
// на один и тот же MonsterType по id. Это Type Object из списка паттернов.
//
// Почему не наследование (class Goblin : public Monster)?
//   - гоблин и крыса отличаются ДАННЫМИ (hp, damage, color, behavior), а не
//     поведением класса. Наследование добавило бы 100 классов с одинаковым
//     кодом;
//   - типы загружаются из JSON — значит, их набор определяется данными,
//     а не компиляцией. Новый монстр = новая запись в JSON, не новый .cpp.
//
// Почему behaviorId — строка, а не enum:
//   - Behavior живёт в BehaviorRegistry по строковому ключу. Строка позволяет
//     добавлять поведения без правки enum и без перекомпиляции;
//   - цена — поиск в unordered_map. Для нескольких десятков монстров
//     неощутимо.
// ---------------------------------------------------------------------------
struct MonsterType
{
    std::string id;          // "goblin", "rat", "orc"
    std::string name;        // "Гоблин" — для лога
    std::string behaviorId;  // "aggressive", "flee", "patrol"

    int hp          = 5;
    int damage      = 1;
    int visionRange = 8;     // с какого расстояния замечает игрока

    sf::Color color     = sf::Color::White;
    float     sizeRatio = 0.875f;
};
