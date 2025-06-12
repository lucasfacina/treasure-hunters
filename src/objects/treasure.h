#ifndef TREASURE_OBJECT_H
#define TREASURE_OBJECT_H
#include "game_object.h"

class TreasureObject final : public GameObject {
    int value;

public:
    explicit TreasureObject(
        const int tileId,
        const int value,
        const int startX,
        const int startY)
        : GameObject(tileId, startX, startY)
          , value(value) {
    }

    [[nodiscard]] int getValue() const { return value; }
};

#endif //TREASURE_OBJECT_H
