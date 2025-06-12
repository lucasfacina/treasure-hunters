#ifndef TREASURE_H
#define TREASURE_H
#include "game_object.h"

class Treasure final : public GameObject {
    int value;

public:
    explicit Treasure(
        const int tileId,
        const int value,
        const int startX,
        const int startY)
        : GameObject(tileId, startX, startY)
          , value(value) {
    }
};

#endif //TREASURE_H
