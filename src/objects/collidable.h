#ifndef COLLIDABLE_OBJECT_H
#define COLLIDABLE_OBJECT_H
#include "game_object.h"

class CollidableObject : public GameObject {
public:
    explicit CollidableObject(const int x, const int y)
        : GameObject(x, y) {
    }
};

#endif //COLLIDABLE_OBJECT_H
