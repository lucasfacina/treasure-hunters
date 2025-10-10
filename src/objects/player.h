#ifndef PLAYER_H
#define PLAYER_H
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#include "game_object.h"

#define PLAYER_SPEED 1 // tile per update

struct MovimentKeyMap {
    int up;
    int down;
    int left;
    int right;
    char serialUp;
    char serialDown;
    char serialLeft;
    char serialRight;
};

class Player final : public GameObject {
    ALLEGRO_BITMAP *sprite;
    MovimentKeyMap keyMap;
    std::shared_ptr<TreasureObject> holdingTreasureItem;

    PlayerType type;
    char serialCommand = '0';

    bool keyUp{false};
    bool keyDown{false};
    bool keyLeft{false};
    bool keyRight{false};

    bool check_collision(
        const int targetX,
        const int targetY) override {
        bool collision = false;

        for (const auto &obj: this->mapManager->findGameObjectsAt(targetX, targetY)) {
            if (const auto treasure = std::dynamic_pointer_cast<TreasureObject>(obj))
                if (holdingTreasureItem == nullptr && !treasure->isHeld()) {
                    treasure->setHoldingBy(shared_from_this());
                    holdingTreasureItem = treasure;
                }

            if (const auto chest = std::dynamic_pointer_cast<ChestObject>(obj)) {
                if (holdingTreasureItem != nullptr)
                    this->saveTreasureItem(chest);
                collision = true;
            }

            if (std::dynamic_pointer_cast<CollidableObject>(obj))
                collision = true;
        }

        return collision;
    }

    void saveTreasureItem(const std::shared_ptr<ChestObject> &chest) {
        if (this->mapManager->storeTreasure(holdingTreasureItem, chest)) {
            holdingTreasureItem->removeHolding();
            holdingTreasureItem = nullptr;
        }
    }

    void update_position() {
        const auto oldX = this->x;
        const auto oldY = this->y;

        if (keyUp) this->move(0.0f, -PLAYER_SPEED);
        if (keyDown) this->move(0.0f, PLAYER_SPEED);
        if (keyLeft) this->move(-PLAYER_SPEED, 0.0f);
        if (keyRight) this->move(PLAYER_SPEED, 0.0f);

        if (holdingTreasureItem != nullptr)
            holdingTreasureItem->move(this->x - oldX, this->y - oldY);
    }

    bool isKeyActive(const ALLEGRO_KEYBOARD_STATE *key_state, int keyboardKey, char serialKey) const {
        return al_key_down(key_state, keyboardKey) || (serialCommand != '0' && serialCommand == serialKey);
    }

public:
    explicit Player(
        PlayerType type,
        ALLEGRO_BITMAP *sprite,
        const MovimentKeyMap &keyMap,
        const int startX,
        const int startY)
        : GameObject(-1, startX, startY)
          , type(type)
          , sprite(sprite)
          , keyMap(keyMap)
          , holdingTreasureItem(nullptr) {}

    void setSerialCommand(char command) { serialCommand = command; }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) override {
        if (event->type != ALLEGRO_EVENT_TIMER) return;

        this->keyUp = isKeyActive(key_state, keyMap.up, keyMap.serialUp);
        this->keyDown = isKeyActive(key_state, keyMap.down, keyMap.serialDown);
        this->keyLeft = isKeyActive(key_state, keyMap.left, keyMap.serialLeft);
        this->keyRight = isKeyActive(key_state, keyMap.right, keyMap.serialRight);

        this->update_position();
    }

    void draw() const override {
        al_draw_bitmap(this->sprite, x * Settings::TILE_SIZE, y * Settings::TILE_SIZE, 0);
    }
};

#endif //PLAYER_H
