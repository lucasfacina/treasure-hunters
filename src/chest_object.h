#ifndef CHEST_OBJECT_H
#define CHEST_OBJECT_H
#include "game_object.h"

class ChestObject final : public GameObject {
public:
    explicit ChestObject(const int x, const int y)
        : GameObject(x, y) {
    }
};

#endif //CHEST_OBJECT_H
