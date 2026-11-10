#pragma once

#include "Command.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <memory>
#include <unordered_map>

class InputHandler
{
public:
    InputHandler()
    {
        // Маппинг клавиша → фабрика команды.
        // std::function или лямбда — чтобы не плодить пулы команд.
        bindings_[sf::Keyboard::Key::W] = [] { return std::make_unique<MoveCommand>( 0, -1); };
		bindings_[sf::Keyboard::Key::S] = [] { return std::make_unique<MoveCommand>( 0, +1); };
		bindings_[sf::Keyboard::Key::A] = [] { return std::make_unique<MoveCommand>(-1,  0); };
		bindings_[sf::Keyboard::Key::D] = [] { return std::make_unique<MoveCommand>(+1,  0); };
		bindings_[sf::Keyboard::Key::Space] = [] { return std::make_unique<WaitCommand>(); };
    }

    // Возвращает true, если команда была выполнена (ход потрачен).
    bool handleEvent(const sf::Event& event, GameContext& ctx)
    {
        if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        {
            auto it = bindings_.find(key->code);
            if (it != bindings_.end())
            {
                auto cmd = it->second();
                return cmd->execute(ctx);
            }
        }
        return false;
    }

private:
    using CommandFactory = std::function<std::unique_ptr<Command>()>;
    std::unordered_map<sf::Keyboard::Key, CommandFactory> bindings_;
};
