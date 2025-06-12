#ifndef PLAYER_H
#define PLAYER_H
#include <format>
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#include "game_object.h"

#define PLAYER_SPEED 1

struct MovimentKeyMap {
    int up;
    int down;
    int left;
    int right;
};

class Player final : public GameObject {
    std::vector<std::shared_ptr<GameObject> > gameObjects;

    ALLEGRO_BITMAP *sprite;
    MovimentKeyMap keyMap;
    std::shared_ptr<Treasure> holdingTreasureItem;

    bool keyUp{false};
    bool keyDown{false};
    bool keyLeft{false};
    bool keyRight{false};

    bool check_collision(
        const int nextX,
        const int nextY,
        const std::vector<std::shared_ptr<GameObject> > &gameObj) override {
        for (const auto &obj: gameObj) {
            if (obj == holdingTreasureItem) return false; // Não colidiu com o próprio tesouro que está sendo segurado

            std::cout << std::format("x ({}) -> {} | {}", x, nextX, obj->getX()) << std::endl;
            std::cout << std::format("y ({}) -> {} | {}", y, nextY, obj->getY()) << std::endl;
            std::cout << "=======================================" << std::endl;

            if (nextX != obj->getX() && nextY != obj->getY()) return false; // Não há colisão com este objeto

            if (const auto treasure = std::dynamic_pointer_cast<Treasure>(obj))
                if (holdingTreasureItem == nullptr) {
                    holdingTreasureItem = treasure;
                    return false; // Colisão com o tesouro, o jogador ficará em cima dele
                }

            if (std::dynamic_pointer_cast<ChestObject>(obj) && holdingTreasureItem != nullptr) {
                std::cout << "Guardou o tesouro" << std::endl;
                holdingTreasureItem = nullptr;
                return true; // Colisão com o baú, o jogador guarda o tesouro
            }

            return true; // Colisão com outro objeto
        }
        return false; // Não há colisão com nenhum objeto
    }

    void update_position() {
        if (keyUp) this->move(0.0f, -PLAYER_SPEED, this->gameObjects);
        if (keyDown) this->move(0.0f, PLAYER_SPEED, this->gameObjects);
        if (keyLeft) this->move(-PLAYER_SPEED, 0.0f, this->gameObjects);
        if (keyRight) this->move(PLAYER_SPEED, 0.0f, this->gameObjects);

        if (holdingTreasureItem != nullptr)
            holdingTreasureItem->setPosition(this->x, this->y);
    }

public:
    explicit Player(
        ALLEGRO_BITMAP *sprite,
        const MovimentKeyMap &keyMap,
        const int startX,
        const int startY,
        const std::vector<std::shared_ptr<GameObject> > &gameObjects
    )
        : GameObject(-1, startX, startY)
          , sprite(sprite)
          , keyMap(keyMap)
          , gameObjects(gameObjects)
          , holdingTreasureItem(nullptr) {
    }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) override {
        if (event->type != ALLEGRO_EVENT_TIMER) return;

        this->keyUp = al_key_down(key_state, keyMap.up);
        this->keyDown = al_key_down(key_state, keyMap.down);
        this->keyLeft = al_key_down(key_state, keyMap.left);
        this->keyRight = al_key_down(key_state, keyMap.right);

        this->update_position();
    }

    void draw() const override {
        al_draw_bitmap(this->sprite, x * Settings::TILE_SIZE, y * Settings::TILE_SIZE, 0);
    }
};

#endif //PLAYER_H
