#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <iostream>
#include <allegro5/bitmap_draw.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>

#include "utils/settings.h"

class MapManager;

class GameObject : public std::enable_shared_from_this<GameObject> {
protected:
    std::shared_ptr<MapManager> mapManager;
    int x;
    int y;
    bool selfRender;

    int tileId;

public:
    virtual ~GameObject() = default;

    explicit GameObject(
        const int startX,
        const int startY)
        : x(startX)
          , y(startY)
          , selfRender(false)
          , tileId(-1) {}

    explicit GameObject(
        const int tileId,
        const int startX,
        const int startY)
        : x(startX)
          , y(startY)
          , selfRender(true)
          , tileId(tileId) {}

    void move(int offsetX, int offsetY);

    void setPosition(const int newX, const int newY) {
        x = newX;
        y = newY;
    }

    virtual bool check_collision(const int targetX, const int targetY) {
        return false;
    }

    virtual void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {}

    virtual void draw() const {
        if (!selfRender) return;

        const float sx = static_cast<float>(tileId % Settings::TILE_COLS) * Settings::TILE_SIZE;
        const float sy = static_cast<float>(tileId / Settings::TILE_COLS) * Settings::TILE_SIZE;
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

    [[nodiscard]] bool isSelfRender() const { return selfRender; }
    [[nodiscard]] int getX() const { return x; }
    [[nodiscard]] int getY() const { return y; }

    void injectMapManager(const std::shared_ptr<MapManager> &mapManager) {
        this->mapManager = mapManager;
    }
};

#endif //GAME_OBJECT_H
