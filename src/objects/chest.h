#ifndef CHEST_OBJECT_H
#define CHEST_OBJECT_H
#include "game_object.h"
#include "utils/player_type.h"

class ChestObject final : public GameObject {
    PlayerType playerType;

public:
    explicit ChestObject(PlayerType playerType, const int x, const int y)
        : GameObject(x, y)
    , playerType(playerType){
    }
};

#endif //CHEST_OBJECT_H
