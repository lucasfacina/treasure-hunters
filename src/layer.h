#ifndef LAYER_H
#define LAYER_H
#include <cstdio>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>

#include "game_object.h"
#include "object_factory.h"
#include "sprites.h"

class Layer {
    int **tiles;

    int mapWidth;
    int mapHeight;

public:
    explicit Layer(
        const int mapWidth,
        const int mapHeight)
        : mapWidth(mapWidth)
          , mapHeight(mapHeight) {
        tiles = new int *[mapWidth];
        for (int i = 0; i < mapWidth; ++i) {
            tiles[i] = new int[mapHeight];
            for (int j = 0; j < mapHeight; ++j) {
                tiles[i][j] = -1;
            }
        }
    }

    ~Layer() {
        for (int i = 0; i < mapWidth; ++i)
            delete[] tiles[i];
        delete[] tiles;
    }

    void load(const char *filename, std::vector<std::shared_ptr<GameObject> > &gameObjects) const {
        FILE *file = fopen(filename, "r");
        if (!file) {
            printf("Erro ao abrir o arquivo de assets: %s\n", filename);
            return;
        }

        char line[4096];
        int row = 0;
        while (fgets(line, sizeof(line), file) && row < this->mapHeight) {
            int col = 0;
            const char *token = strtok(line, ",");
            while (token != nullptr && col < this->mapWidth) {
                const int tileIndex = atoi(token);

                auto gameObj = createById(tileIndex, col, row);

                if (gameObj != nullptr) {
                    gameObjects.push_back(gameObj);

                    if (!gameObj->isSelfRender())
                    this->setTileAt(col, row, tileIndex);
                } else {
                    this->setTileAt(col, row, tileIndex);
                }

                token = strtok(nullptr, ",");
                col++;
            }
            row++;
        }
        fclose(file);
    }

    bool isValidTileIndex(const int tileIndex) const {
        const int tileset_cols = al_get_bitmap_width(Settings::spritesheet) / Settings::TILE_SIZE;
        const int tileset_rows = al_get_bitmap_height(Settings::spritesheet) / Settings::TILE_SIZE;
        return tileIndex >= -1 && tileIndex < tileset_cols * tileset_rows;
    }

    bool isValidCoord(const int x, const int y) const {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }

    void setTileAt(const int x, const int y, const int tileIndex) const {
        if (!this->isValidCoord(x, y)) return;
        if (!this->isValidTileIndex(tileIndex)) return;

        this->tiles[x][y] = tileIndex;
    }

    void draw() const {
        for (int y = 0; y < this->mapHeight; ++y) {
            for (int x = 0; x < this->mapWidth; ++x) {
                const int tile_index = this->tiles[x][y];
                if (tile_index < 0) continue;

                const float sx = static_cast<float>(tile_index % Settings::TILE_COLS) * Settings::TILE_SIZE;
                const float sy = static_cast<float>(tile_index / Settings::TILE_COLS) * Settings::TILE_SIZE;
                const float dx = x * Settings::TILE_SIZE;
                const float dy = y * Settings::TILE_SIZE;
                al_draw_bitmap_region(
                    Settings::spritesheet,
                    sx, sy,
                    Settings::TILE_SIZE, Settings::TILE_SIZE,
                    dx, dy,
                    0
                );
            }
        }
    }
};

#endif //LAYER_H
