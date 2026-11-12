#pragma once

#include <deque>
#include <string>
#include <string_view>

// Игровой лог. Хранит последние N сообщений.
// Свежие — в конце. При переполнении старые вытесняются.
//
// Почему std::deque, а не vector:
//   - push_back O(1) амортизированно в обоих;
//   - pop_front у deque O(1), у vector O(N) (сдвиг);
//   - deque не инвалидирует ссылки при push_back (у vector — может).
//
// Почему фиксированный размер:
//   - ограниченная память;
//   - «старое» не интересно (в roguelike — только последние 5-10 строк).
class MessageLog
{
public:
    static constexpr std::size_t kDefaultCapacity = 8;

    explicit MessageLog(std::size_t capacity = kDefaultCapacity)
        : capacity_(capacity) {}

    void push(std::string msg)
    {
        if (msg.empty()) return;
        lines_.push_back(std::move(msg));
        while (lines_.size() > capacity_)
            lines_.pop_front();
    }

    [[nodiscard]] const std::deque<std::string>& lines() const noexcept
    {
        return lines_;
    }

    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }

    void clear() { lines_.clear(); }

private:
    std::size_t capacity_;
    std::deque<std::string> lines_;
};
