#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <vector>

#include "screens/game_screen.h"
#include "screens/screen.h"

class GameManager {
    std::vector<std::shared_ptr<Screen>> screenStack;

public:
    explicit GameManager()
        : screenStack({std::make_shared<GameScreen>()}) {}

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {
        if (!screenStack.empty())
            screenStack.back()->update(event, key_state);
    }

    bool pushScreen(const std::shared_ptr<Screen> &screen) {
        if (screenStack.back() == screen)
            return false;

        screenStack.push_back(screen);
        return true;
    }

    bool popScreen() {
        if (screenStack.size() <= 1)
            return false;

        screenStack.pop_back();
        return true;
    }

    void draw() const {
        for (const auto &screen: screenStack)
            screen->draw();
    }
};

#endif // GAME_MANAGER_H
