#include "game_object.h"
#include "managers/map_manager.h"

void GameObject::move(const int offsetX, const int offsetY) {
    if (!this->mapManager) return;

    const auto targetX = this->x + offsetX;
    const auto targetY = this->y + offsetY;

    if (this->check_collision(targetX, targetY))
        return;

    const auto obj = shared_from_this();
    if (!obj) return;

    this->mapManager->moveGameObject(obj, targetX, targetY);
}
