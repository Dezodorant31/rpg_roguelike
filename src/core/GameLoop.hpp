#pragma once

#include <chrono>

// Игровой цикл с фиксированным шагом.
// Пока в пошаговой игре update() вызывается редко (по командам),
// но архитектура готова к анимациям и физике.
class GameLoop
{
public:
    using Clock     = std::chrono::steady_clock;
    using Duration  = Clock::duration;
    using TimePoint = Clock::time_point;

    // 60 Hz симуляция
    static constexpr Duration kFixedStep = std::chrono::milliseconds(16);

    // Ограничение на накопление: если лагнуло на 1 секунду,
    // не пытаемся догнать 60 шагов подряд (спираль смерти).
    static constexpr Duration kMaxFrameTime = std::chrono::milliseconds(250);

    GameLoop() : last_(Clock::now()) {}

    // Возвращает число «шагов симуляции», которые надо сделать.
    // Вызывается раз в кадр.
    [[nodiscard]] int advance()
    {
        const auto now = Clock::now();
        auto frameTime = now - last_;
        last_ = now;

        if (frameTime > kMaxFrameTime)
            frameTime = kMaxFrameTime;

        accumulator_ += frameTime;

        int steps = 0;
        while (accumulator_ >= kFixedStep)
        {
            accumulator_ -= kFixedStep;
            ++steps;
            if (steps >= 8) { accumulator_ = Duration::zero(); break; } // защита
        }
        return steps;
    }

private:
    TimePoint last_;
    Duration  accumulator_{};
};
