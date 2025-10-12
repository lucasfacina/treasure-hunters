#ifndef TRABALHO_TILEMAP_SCREEN_H
#define TRABALHO_TILEMAP_SCREEN_H
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <memory>

class GameManager;

class Screen {
protected:
    std::shared_ptr<GameManager> gameManager;

public:
    virtual ~Screen() = default;

    virtual void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) = 0;

    virtual void draw() = 0;

    virtual void init() = 0;

    virtual void onFocus() = 0;

    virtual void onLoseFocus() = 0;

    virtual void onExit() = 0;

    void injectGameManager(const std::shared_ptr<GameManager> &gameManager) {
        this->gameManager = gameManager;
    }
};

#endif //TRABALHO_TILEMAP_SCREEN_H
