class PlayerTurnState final : public GameState
{
public:
    // Кто будет «следующим» после потраченного хода
    explicit PlayerTurnState(GameState* next) : next_(next) {}

    bool handleCommand(Command& cmd, GameContext& ctx) override
    {
        if (!ctx.world || !ctx.world->isAlive(ctx.player))
            return false;
        const bool turnTaken = cmd.execute(ctx);
        if (turnTaken) turnConsumed_ = true;
        return turnTaken;
    }

    GameState* update(GameContext&) override
    {
        if (turnConsumed_)
        {
            turnConsumed_ = false;
            return next_;      // ← кто-то снаружи сказал, кто следующий
        }
        return nullptr;
    }

    [[nodiscard]] const char* name() const override { return "PlayerTurn"; }

private:
    GameState* next_ = nullptr;
    bool turnConsumed_ = false;
};
