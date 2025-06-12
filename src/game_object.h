#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <iostream>

class GameObject {
protected:
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
          , tileId(-1) {
    }

    explicit GameObject(
        const int tileId,
        const int startX,
        const int startY)
        : x(startX)
          , y(startY)
          , selfRender(true)
          , tileId(tileId) {
    }

    void move(const int offsetX, const int offsetY, const std::vector<std::shared_ptr<GameObject> > &gameObjects) {
        const auto newX = this->x + offsetX;
        const auto newY = this->y + offsetY;

        if (!this->check_collision(newX, newY, gameObjects)) {
            x = newX;
            y = newY;
        }
    }

    void setPosition(const int newX, const int newY) {
        x = newX;
        y = newY;
    }

    virtual bool check_collision(const int x, const int y, const std::vector<std::shared_ptr<GameObject> > &) {
        return false;
    }

    virtual void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {
    }

    virtual void draw() const {
        if (!selfRender) return;

        const float sx = static_cast<float>(tileId % GlobalSprites::TILE_COLS) * GlobalSprites::TILE_SIZE;
        const float sy = static_cast<float>(tileId / GlobalSprites::TILE_COLS) * GlobalSprites::TILE_SIZE;
        const float dx = x * GlobalSprites::TILE_SIZE;
        const float dy = y * GlobalSprites::TILE_SIZE;

        al_draw_bitmap_region(
            GlobalSprites::spritesheet,
            sx, sy,
            GlobalSprites::TILE_SIZE, GlobalSprites::TILE_SIZE,
            dx, dy,
            0
        );
    }

    [[nodiscard]] bool isSelfRender() const { return selfRender; }
    [[nodiscard]] int getX() const { return x; }
    [[nodiscard]] int getY() const { return y; }
};

#endif //GAME_OBJECT_H
