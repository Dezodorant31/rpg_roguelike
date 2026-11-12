#include "world/BSPDungeonBuilder.hpp"
#include "world/Map.hpp"

#include <algorithm>
#include <cassert>
#include <deque>
#include <random>

namespace
{
    constexpr int kMinLeafSize = 8;    // минимальный размер листа, чтобы делить
    constexpr int kMaxDepth    = 6;    // максимум уровней рекурсии
    constexpr int kMinRoomSize = 3;    // минимальная комната
    constexpr int kRoomPadding = 1;    // отступ от стен листа

    // Случайное число в [lo, hi] включительно.
    // Если hi < lo — меняем местами (защита от неверных входов).
    int randInt(std::mt19937& rng, int lo, int hi)
    {
        if (hi < lo) std::swap(lo, hi);
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(rng);
    }
}

// ---------------------------------------------------------------------------
// Рекурсивное деление узла BSP.
// Каждый вызов создаёт двух детей и рекурсивно делит их.
// Узлы хранятся в std::deque<Node> — deque не инвалидирует указатели
// при push_back, что критично: у нас Node* left/right.
// ---------------------------------------------------------------------------
void BSPDungeonBuilder::splitNode(std::deque<Node>& nodes,
                                  Node& node,
                                  std::mt19937& rng,
                                  int depth)
{
    if (depth >= kMaxDepth) return;

    // Слишком маленький — не делим.
    const int minSplit = kMinLeafSize + kRoomPadding * 2;

    // Решаем, как делить:
    //   - широкий лист → вертикально
    //   - высокий → горизонтально
    //   - квадратный → случайно
    bool splitVertically;
    if (node.bounds.w > node.bounds.h * 5 / 4)
        splitVertically = true;
    else if (node.bounds.h > node.bounds.w * 5 / 4)
        splitVertically = false;
    else
        splitVertically = (rng() & 1u) != 0;

    if (splitVertically)
    {
        if (node.bounds.w < minSplit * 2) return;

        const int splitX = randInt(rng,
            node.bounds.x + minSplit,
            node.bounds.right() - minSplit);

        nodes.push_back(Node{});
        nodes.back().bounds = Rect{node.bounds.x, node.bounds.y,
                                   splitX - node.bounds.x, node.bounds.h};
        Node* left = &nodes.back();

        nodes.push_back(Node{});
        nodes.back().bounds = Rect{splitX, node.bounds.y,
                                   node.bounds.right() - splitX, node.bounds.h};
        Node* right = &nodes.back();

        node.left  = left;
        node.right = right;
    }
    else
    {
        if (node.bounds.h < minSplit * 2) return;

        const int splitY = randInt(rng,
            node.bounds.y + minSplit,
            node.bounds.bottom() - minSplit);

        nodes.push_back(Node{});
        nodes.back().bounds = Rect{node.bounds.x, node.bounds.y,
                                   node.bounds.w, splitY - node.bounds.y};
        Node* left = &nodes.back();

        nodes.push_back(Node{});
        nodes.back().bounds = Rect{node.bounds.x, splitY,
                                   node.bounds.w, node.bounds.bottom() - splitY};
        Node* right = &nodes.back();

        node.left  = left;
        node.right = right;
    }

    splitNode(nodes, *node.left,  rng, depth + 1);
    splitNode(nodes, *node.right, rng, depth + 1);
}

// ---------------------------------------------------------------------------
// В каждом листе — комната. Комната — подпрямоугольник листа,
// отступив от стен на kRoomPadding.
// ---------------------------------------------------------------------------
void BSPDungeonBuilder::createRooms(Node& node,
                                    std::mt19937& rng,
                                    Dungeon& dungeon)
{
    if (node.isLeaf())
    {
        const int maxW = node.bounds.w - kRoomPadding * 2;
        const int maxH = node.bounds.h - kRoomPadding * 2;

        if (maxW < kMinRoomSize || maxH < kMinRoomSize) return;

        const int w = randInt(rng, kMinRoomSize, maxW);
        const int h = randInt(rng, kMinRoomSize, maxH);

        const int x = randInt(rng,
            node.bounds.x + kRoomPadding,
            node.bounds.right() - kRoomPadding - w);
        const int y = randInt(rng,
            node.bounds.y + kRoomPadding,
            node.bounds.bottom() - kRoomPadding - h);

        node.hasRoom    = true;
        node.roomBounds = Rect{x, y, w, h};

        Room room;
        room.bounds = node.roomBounds;
        room.id     = static_cast<int>(dungeon.rooms.size());
        dungeon.rooms.push_back(room);

        // Прорубаем пол
        for (int dy = 0; dy < h; ++dy)
            for (int dx = 0; dx < w; ++dx)
                dungeon.map.at(x + dx, y + dy) = makeFloor();

        return;
    }

    if (node.left)  createRooms(*node.left,  rng, dungeon);
    if (node.right) createRooms(*node.right, rng, dungeon);
}

// ---------------------------------------------------------------------------
// Найти центр комнаты в поддереве.
// Возвращает false, если в поддереве нет ни одной комнаты.
// ---------------------------------------------------------------------------
bool BSPDungeonBuilder::findRoomCenter(const Node& node,
                                       int& outX, int& outY)
{
    if (node.hasRoom)
    {
        outX = node.roomBounds.centerX();
        outY = node.roomBounds.centerY();
        return true;
    }
    if (node.left  && findRoomCenter(*node.left,  outX, outY)) return true;
    if (node.right && findRoomCenter(*node.right, outX, outY)) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Соединяем комнаты коридорами: для каждого внутреннего узла берём
// центр левой и правой комнат и прорубаем L-коридор.
// ---------------------------------------------------------------------------
void BSPDungeonBuilder::connectNodes(Node& node,
                                     std::mt19937& rng,
                                     Dungeon& dungeon)
{
    if (node.isLeaf()) return;

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (node.left  && findRoomCenter(*node.left,  x1, y1)
        && node.right && findRoomCenter(*node.right, x2, y2))
    {
        carveCorridor(dungeon, x1, y1, x2, y2, rng);
    }

    if (node.left)  connectNodes(*node.left,  rng, dungeon);
    if (node.right) connectNodes(*node.right, rng, dungeon);
}

// ---------------------------------------------------------------------------
// L-коридор между двумя точками.
// Случайно выбираем, идти сначала по X или сначала по Y.
// ---------------------------------------------------------------------------
void BSPDungeonBuilder::carveCorridor(Dungeon& dungeon,
                                      int x1, int y1,
                                      int x2, int y2,
                                      std::mt19937& rng)
{
    const bool horizontalFirst = (rng() & 1u) != 0;

    auto carveH = [&](int xa, int xb, int y) {
        for (int x = std::min(xa, xb); x <= std::max(xa, xb); ++x)
            if (dungeon.map.inBounds(x, y))
                dungeon.map.at(x, y) = makeFloor();
    };
    auto carveV = [&](int ya, int yb, int x) {
        for (int y = std::min(ya, yb); y <= std::max(ya, yb); ++y)
            if (dungeon.map.inBounds(x, y))
                dungeon.map.at(x, y) = makeFloor();
    };

    if (horizontalFirst)
    {
        carveH(x1, x2, y1);
        carveV(y1, y2, x2);
    }
    else
    {
        carveV(y1, y2, x1);
        carveH(x1, x2, y2);
    }
}

// ---------------------------------------------------------------------------
// Точка входа: собрать всё вместе.
// ---------------------------------------------------------------------------
Dungeon BSPDungeonBuilder::build(std::uint32_t seed, int width, int height)
{
    assert(width > 20 && height > 20 && "Карта слишком мала для BSP");

    Dungeon dungeon;
    dungeon.map  = Map(width, height);
    dungeon.map.fill(TileType::Wall);
    dungeon.seed = seed;

    std::mt19937 rng(seed);

    std::deque<Node> nodes;
    nodes.push_back(Node{});
    nodes.back().bounds = Rect{1, 1, width - 2, height - 2};

    Node& root = nodes.front();        // ← ССЫЛКА НА КОРЕНЬ (не back!)
    splitNode(nodes, root, rng, 0);
    createRooms(root, rng, dungeon);   // ← обходим ВСЁ дерево от корня
    connectNodes(root, rng, dungeon);  // ← соединяем ВСЁ дерево от корня

    //placeDoors(dungeon);

    if (!dungeon.rooms.empty())
    {
        dungeon.rooms[0].type = Room::Type::Entrance;
        dungeon.playerSpawnX  = dungeon.rooms[0].centerX();
        dungeon.playerSpawnY  = dungeon.rooms[0].centerY();
    }

    for (auto& t : dungeon.map.tiles())
    {
        t.color = (t.type == TileType::Floor) ? sf::Color(60, 60, 70)
              //: (t.type == TileType::Door)    ? sf::Color(140, 100, 40)
                                              : sf::Color(30, 30, 35);
    }

    return dungeon;
}
