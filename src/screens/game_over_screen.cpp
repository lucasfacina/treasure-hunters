#include "managers/game_manager.h"
#include "game_over_screen.h"

#include <allegro5/events.h>
#include <allegro5/keyboard.h>

void GameOverScreen::update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {
    if (al_key_down(key_state, ALLEGRO_KEY_ENTER) ||
        al_key_down(key_state, ALLEGRO_KEY_ESCAPE)) {
        this->gameManager->popScreen();
    }
}
