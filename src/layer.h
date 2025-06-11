#ifndef LAYER_H
#define LAYER_H
#include <cstdio>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>

class Layer {
    ALLEGRO_BITMAP *tilemap;
    int **tiles;
    bool isCollidable;

    int tileSize;
    int mapWidth;
    int mapHeight;

public:
    explicit Layer(
        ALLEGRO_BITMAP *tilemap,
        const int mapWidth,
        const int mapHeight,
        const int tileSize,
        const bool collidable)
        : tilemap(tilemap)
          , isCollidable(collidable)
          , tileSize(tileSize)
          , mapWidth(mapWidth)
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

    void load(const char *filename) {
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
                this->setTileAt(col, row, atoi(token));
                token = strtok(nullptr, ",");
                col++;
            }
            row++;
        }
        fclose(file);
    }

    bool isValidTileIndex(const int tileIndex) const {
        const int tileset_cols = al_get_bitmap_width(this->tilemap) / this->tileSize;
        const int tileset_rows = al_get_bitmap_height(this->tilemap) / this->tileSize;
        return tileIndex >= -1 && tileIndex < tileset_cols * tileset_rows;
    }

    bool isValidCoord(const int x, const int y) const {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }

    void setTileAt(const int x, const int y, const int tileIndex) {
        if (!this->isValidCoord(x, y)) return;
        if (!this->isValidTileIndex(tileIndex)) return;

        this->tiles[x][y] = tileIndex;
    }

    void draw() const {
        if (!tilemap) return;

        const int tileset_cols = al_get_bitmap_width(this->tilemap) / this->tileSize;
        for (int y = 0; y < this->mapHeight; ++y) {
            for (int x = 0; x < this->mapWidth; ++x) {
                const int tile_index = this->tiles[x][y];
                if (tile_index < 0) continue;

                const float sx = static_cast<float>(tile_index % tileset_cols) * this->tileSize;
                const float sy = static_cast<float>(tile_index / tileset_cols) * this->tileSize;
                const float dx = x * this->tileSize;
                const float dy = y * this->tileSize;
                al_draw_bitmap_region(this->tilemap, sx, sy, this->tileSize, this->tileSize, dx, dy, 0);
            }
        }
    }
};

#endif //LAYER_H
