#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
#include <vector>

#include "layer.h"

class MapManager {
    ALLEGRO_BITMAP *tilemap;
    std::vector<std::shared_ptr<Layer> > layers;

    int mapWidth;
    int mapHeight;
    int tileSize;

public:
    explicit MapManager(
        const int mapWidth,
        const int mapHeight,
        const int tileSize)
        : mapWidth(mapWidth)
          , mapHeight(mapHeight)
          , tileSize(tileSize) {
        this->tilemap = al_load_bitmap("assets/cenario.png");
    }

    ~MapManager() {
        if (this->tilemap)
            al_destroy_bitmap(this->tilemap);

        this->layers.clear();
    }

    void addLayer(std::shared_ptr<Layer> layer) {
        if (layer)
            this->layers.push_back(layer);
    }

    void addLayer(const char *csv_map_file, bool collidable = false) {
        auto new_layer = std::make_shared<Layer>(
            this->tilemap,
            this->mapWidth,
            this->mapHeight,
            this->tileSize,
            collidable
        );

        new_layer->load(csv_map_file);

        this->layers.push_back(new_layer);
    }

    void draw() {
        for (const auto layer: this->layers)
            layer->draw();
    }
};

#endif //MAP_MANAGER_H
