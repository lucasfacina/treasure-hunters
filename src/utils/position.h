#ifndef POSITION_H
#define POSITION_H
#include <cstddef>
#include <functional>

struct Position {
    int x;
    int y;

    bool operator==(const Position &other) const {
        return x == other.x && y == other.y;
    }
};

// Estrutura de Hash para a Posição (necessário para o unordered_map)
// Isso "ensina" o unordered_map a converter uma Posição num valor de hash.
struct PositionHash {
    std::size_t operator()(const Position &p) const {
        auto hash1 = std::hash<int>{}(p.x);
        auto hash2 = std::hash<int>{}(p.y);

        // Combina os hashes. O valor 0x9e3779b9 é frequentemente usado para melhorar a distribuição.
        return hash1 ^ (hash2 << 1);
    }
};

#endif //POSITION_H
