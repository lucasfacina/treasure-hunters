#ifndef LAYER_H
#define LAYER_H
#include <cstdio>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>

#include "../objects/game_object.h"
#include "object_factory.h"
#include "settings.h"

class Layer {
    int **tiles;

public:
    explicit Layer() {
        tiles = new int *[Settings::MAP_WIDTH];
        for (int i = 0; i < Settings::MAP_WIDTH; ++i) {
            tiles[i] = new int[Settings::MAP_HEIGHT];
            for (int j = 0; j < Settings::MAP_HEIGHT; ++j) {
                tiles[i][j] = -1;
            }
        }
    }

    ~Layer() {
        for (int i = 0; i < Settings::MAP_WIDTH; ++i)
            delete[] tiles[i];
        delete[] tiles;
    }

    void load(const char *filename, std::vector<std::shared_ptr<GameObject>> &gameObjects) const {
        FILE *file = fopen(filename, "r");
        if (!file) {
            printf("Erro ao abrir o arquivo de assets: %s\n", filename);
            return;
        }

        char line[4096];
        int row = 0;
        while (fgets(line, sizeof(line), file) && row < Settings::MAP_HEIGHT) {
            int col = 0;
            const char *token = strtok(line, ",");
            while (token != nullptr && col < Settings::MAP_WIDTH) {
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
        return x >= 0 && x < Settings::MAP_WIDTH && y >= 0 && y < Settings::MAP_HEIGHT;
    }

    void setTileAt(const int x, const int y, const int tileIndex) const {
        if (!this->isValidCoord(x, y)) return;
        if (!this->isValidTileIndex(tileIndex)) return;

        this->tiles[x][y] = tileIndex;
    }

    void draw() const {
        for (int y = 0; y < Settings::MAP_HEIGHT; ++y) {
            for (int x = 0; x < Settings::MAP_WIDTH; ++x) {
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
