
#include "game_object.h"
#include "managers/map_manager.h"

void GameObject::move(const int offsetX, const int offsetY) {
    std::cout << "Ação de mover engatilhada!" << std::endl;
    if (!this->mapManager) return;
    std::cout << "MapManager existe, podemos prosseguir" << std::endl;

    const auto targetX = this->x + offsetX;
    const auto targetY = this->y + offsetY;

    if (this->check_collision(targetX, targetY)) {
        std::cout << "Colisão detectada, não podemos mover!" << std::endl;
        return;
    }

    const auto it = shared_from_this();
    if (!it) return;
    this->mapManager->moveGameObject(it, targetX, targetY);
}
