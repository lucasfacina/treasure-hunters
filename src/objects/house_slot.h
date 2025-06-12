#ifndef HOUSE_SLOT_H
#define HOUSE_SLOT_H
#include "game_object.h"
#include "utils/player_type.h"

class HouseSlotObject final : public GameObject {
    PlayerType playerType;
    std::shared_ptr<TreasureObject> storedTreasureItem;

public:
    explicit HouseSlotObject(
        const PlayerType playerType,
        const int x, const int y)
        : GameObject(x, y)
          , playerType(playerType) {
    }

    bool isEmpty() const { return storedTreasureItem == nullptr; }

    std::shared_ptr<TreasureObject> getStoredTresureItem() { return this->storedTreasureItem; }

    void store(const std::shared_ptr<TreasureObject> &treasureItem) {
        if (!this->isEmpty()) return;

        this->storedTreasureItem = treasureItem;
    }

    [[nodiscard]] PlayerType getType() const { return playerType; }
};

#endif //HOUSE_SLOT_H
