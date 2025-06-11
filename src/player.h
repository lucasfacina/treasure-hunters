#ifndef PLAYER_H
#define PLAYER_H
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#define PLAYER_SPEED 2.0f

struct MovimentKeyMap {
    int up;
    int down;
    int left;
    int right;
};

class Player {
    ALLEGRO_BITMAP *sprite;
    MovimentKeyMap keyMap;
    float x, y;
    int treasureCount;

    bool keyUp{false};
    bool keyDown{false};
    bool keyLeft{false};
    bool keyRight{false};

    bool check_collision(const float xOffset, const float yOffset) {
        // TODO: Implementar lógica de colisão com o mapa
        return false;
    }

    void update_position() {
        if (keyUp && !check_collision(0.0f, -PLAYER_SPEED)) y -= PLAYER_SPEED;
        if (keyDown && !check_collision(0.0f, PLAYER_SPEED)) y += PLAYER_SPEED;
        if (keyLeft && !check_collision(-PLAYER_SPEED, 0.0f)) x -= PLAYER_SPEED;
        if (keyRight && !check_collision(PLAYER_SPEED, 0.0f)) x += PLAYER_SPEED;
    }

public:
    explicit Player(
        ALLEGRO_BITMAP *sprite,
        const MovimentKeyMap &keyMap,
        const float startX = 0.0f,
        const float startY = 0.0f)
        : sprite(sprite)
          , keyMap(keyMap)
          , x(startX)
          , y(startY)
          , treasureCount(0) {
    }

    ~Player() {
        if (sprite)
            al_destroy_bitmap(sprite);
    }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {
        if (event->type != ALLEGRO_EVENT_TIMER) return;

        this->keyUp = al_key_down(key_state, keyMap.up);
        this->keyDown = al_key_down(key_state, keyMap.down);
        this->keyLeft = al_key_down(key_state, keyMap.left);
        this->keyRight = al_key_down(key_state, keyMap.right);

        this->update_position();
    }

    void draw() const {
        al_draw_bitmap(this->sprite, this->x, this->y, 0);
    }
};

#endif //PLAYER_H
