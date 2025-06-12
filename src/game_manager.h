#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "map_manager.h"
#include "player.h"

class GameManager {
    std::shared_ptr<MapManager> map_manager;
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;

public:
    GameManager(
        const int mapWidth,
        const int mapHeight
    ) {
        this->map_manager = std::make_shared<MapManager>(mapWidth, mapHeight);

        this->map_manager->addLayer("assets/mapa_fundo_grama.csv");
        this->map_manager->addLayer("assets/mapa_borda_floresta.csv");
        this->map_manager->addLayer("assets/mapa_caminho.csv");
        this->map_manager->addLayer("assets/mapa_nascente_agua.csv");
        this->map_manager->addLayer("assets/mapa_pedras_arbustos.csv");
        this->map_manager->addLayer("assets/mapa_agua.csv");
        this->map_manager->addLayer("assets/mapa_pontes.csv");
        this->map_manager->addLayer("assets/mapa_casas.csv");
        this->map_manager->addLayer("assets/mapa_tesouros.csv");

        this->player1 = std::make_shared<Player>(
            al_load_bitmap("assets/player1.png"),
            MovimentKeyMap{ALLEGRO_KEY_W, ALLEGRO_KEY_S, ALLEGRO_KEY_A, ALLEGRO_KEY_D},
            8, 3,
            this->map_manager->getGameObjects()
        );

        this->player2 = std::make_shared<Player>(
            al_load_bitmap("assets/player2.png"),
            MovimentKeyMap{ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT},
            41, 3,
            this->map_manager->getGameObjects()
        );
    }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) const {
        player1->update(event, key_state);
        player2->update(event, key_state);
    }

    void draw() const {
        map_manager->draw();
        player1->draw();
        player2->draw();
    }
};

#endif //GAME_MANAGER_H
