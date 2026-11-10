#pragma once

struct Health
{
    int max     = 10;
    int current = 10;

    [[nodiscard]] bool isDead() const noexcept { return current <= 0; }
};
