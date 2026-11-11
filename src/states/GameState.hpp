// GameState.hpp
class GameState
{
public:
    virtual ~GameState() = default;
    virtual bool handleCommand(Command& cmd, GameContext& ctx) = 0;
    virtual GameState* update(GameContext& ctx) = 0;
    [[nodiscard]] virtual const char* name() const = 0;
};
