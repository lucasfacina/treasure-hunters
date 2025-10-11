#ifndef TREASURE_OBJECT_H
#define TREASURE_OBJECT_H
#include "game_object.h"
#include "utils/position.h"

class TreasureObject final : public GameObject {
    int value;

    Position initialPosition;

    std::shared_ptr<GameObject> playerHolding;

public:
    explicit TreasureObject(
        const int tileId,
        const int value,
        const int startX,
        const int startY)
        : GameObject(tileId, startX, startY)
          , value(value)
          , initialPosition({startX, startY})
          , playerHolding(nullptr) {}

    void move(const int offsetX, const int offsetY) override {
        if (!playerHolding) GameObject::move(offsetX, offsetY);
        if (!this->mapManager) return;

        const auto targetX = this->x + offsetX;
        const auto targetY = this->y + offsetY;
        this->setPosition(targetX, targetY);
    }

    [[nodiscard]] Position getInitialPosition() const { return initialPosition; }

    [[nodiscard]] int getValue() const { return value; }

    [[nodiscard]] bool isHeld() const { return playerHolding != nullptr; }

    void setHoldingBy(const std::shared_ptr<GameObject> &player) { playerHolding = player; }

    void removeHolding() { playerHolding = nullptr; }
};

#endif //TREASURE_OBJECT_H
