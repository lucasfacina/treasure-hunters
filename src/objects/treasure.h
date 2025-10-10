#ifndef TREASURE_OBJECT_H
#define TREASURE_OBJECT_H
#include "game_object.h"

class TreasureObject final : public GameObject {
    int value;
    std::shared_ptr<GameObject> playerHolding;

public:
    explicit TreasureObject(
        const int tileId,
        const int value,
        const int startX,
        const int startY)
        : GameObject(tileId, startX, startY)
          , value(value)
          , playerHolding(nullptr) {}

    [[nodiscard]] int getValue() const { return value; }

    [[nodiscard]] bool isHeld() const { return playerHolding != nullptr; }

    void setHoldingBy(const std::shared_ptr<GameObject> &player) { playerHolding = player; }

    void removeHolding() { playerHolding = nullptr; }
};

#endif //TREASURE_OBJECT_H
