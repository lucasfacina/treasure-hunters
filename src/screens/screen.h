#ifndef TRABALHO_TILEMAP_SCREEN_H
#define TRABALHO_TILEMAP_SCREEN_H
#include <allegro5/events.h>
#include <allegro5/keyboard.h>

class Screen {
public:
    virtual ~Screen() = default;

    virtual void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) = 0;

    virtual void draw() = 0;
};

#endif //TRABALHO_TILEMAP_SCREEN_H
