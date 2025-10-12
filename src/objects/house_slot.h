#ifndef HOUSE_SLOT_H
#define HOUSE_SLOT_H
#include "collidable.h"
#include "treasure.h"
#include "utils/player_type.h"

class HouseSlotObject final : public CollidableObject {
    PlayerType playerType;
    std::optional<PlayerType> depositedByPlayerType;
    std::shared_ptr<TreasureObject> storedTreasureItem;

public:
    explicit HouseSlotObject(
        const PlayerType playerType,
        const int x, const int y)
        : CollidableObject(x, y)
          , playerType(playerType)
          , depositedByPlayerType(std::nullopt) {}

    bool isEmpty() const { return storedTreasureItem == nullptr; }

    std::shared_ptr<TreasureObject> getStoredTresureItem() { return this->storedTreasureItem; }

    [[nodiscard]] std::optional<PlayerType> getDepositedBy() const { return this->depositedByPlayerType; }

    void store(const std::shared_ptr<TreasureObject> &treasureItem, const PlayerType depositedBy) {
        if (!this->isEmpty()) return;

        this->storedTreasureItem = treasureItem;
        this->depositedByPlayerType = depositedBy;
    }

    [[nodiscard]] PlayerType getType() const { return playerType; }
};

#endif //HOUSE_SLOT_H
