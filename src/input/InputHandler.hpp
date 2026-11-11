#pragma once

#include "input/Command.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

class InputHandler
{
public:
    InputHandler()
    {
        // Движение
        bindings_[sf::Keyboard::Key::W] = [] { return std::make_unique<MoveCommand>( 0, -1); };
        bindings_[sf::Keyboard::Key::S] = [] { return std::make_unique<MoveCommand>( 0, +1); };
        bindings_[sf::Keyboard::Key::A] = [] { return std::make_unique<MoveCommand>(-1,  0); };
        bindings_[sf::Keyboard::Key::D] = [] { return std::make_unique<MoveCommand>(+1,  0); };

        // Атака в направлении
        bindings_[sf::Keyboard::Key::Up]    = [] { return std::make_unique<AttackCommand>( 0, -1); };
        bindings_[sf::Keyboard::Key::Down]  = [] { return std::make_unique<AttackCommand>( 0, +1); };
        bindings_[sf::Keyboard::Key::Left]  = [] { return std::make_unique<AttackCommand>(-1,  0); };
        bindings_[sf::Keyboard::Key::Right] = [] { return std::make_unique<AttackCommand>(+1,  0); };

        // Ожидание
        bindings_[sf::Keyboard::Key::Space] = [] { return std::make_unique<WaitCommand>(); };
    }

    // Создать команду по клавише. Возвращает nullptr, если клавиша не смаплена.
    // В отличие от handleEvent, не выполняет — отдаёт команду наружу,
    // чтобы GameState решил, принимать ли её.
    [[nodiscard]] std::unique_ptr<Command> makeCommand(const sf::Event& event) const
    {
        if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        {
            auto it = bindings_.find(key->code);
            if (it != bindings_.end())
                return it->second();
        }
        return nullptr;
    }

private:
    using CommandFactory = std::function<std::unique_ptr<Command>()>;
    std::unordered_map<sf::Keyboard::Key, CommandFactory> bindings_;
};
