#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
#include <vector>

#include "../utils/layer.h"

class MapManager {
    std::vector<std::shared_ptr<Layer>> layers;
    std::vector<std::shared_ptr<GameObject>> gameObjects;

public:
    ~MapManager() {
        this->layers.clear();
        this->gameObjects.clear();
    }

    void addLayer(const char *csv_map_file) {
        auto new_layer = std::make_shared<Layer>();

        new_layer->load(csv_map_file, this->gameObjects);

        this->layers.push_back(new_layer);
    }

    void draw() const {
        for (const auto &layer: this->layers)
            layer->draw();

        for (const auto &gameObject: this->gameObjects)
            gameObject->draw();
    }

    std::vector<std::shared_ptr<GameObject>> &getGameObjects() {
        return this->gameObjects;
    }
};

#endif //MAP_MANAGER_H
