#ifndef TRABALHO_TILEMAP_GAME_SCREEN_H
#define TRABALHO_TILEMAP_GAME_SCREEN_H
#include <iostream>

#include "screen.h"
#include "utils/assets.h"
#include "objects/player.h"
#include "managers/serial_port.h"
#include "managers/map_manager.h"

class GameScreen final : public Screen {
    std::shared_ptr<MapManager> map_manager;
    std::shared_ptr<Player> bluePlayer;
    std::shared_ptr<Player> pinkPlayer;

    std::unique_ptr<SerialPort> arduino;
    bool useSerial;

    void initializeSerial() {
        if (!useSerial) return;

        try {
            arduino = std::make_unique<SerialPort>("", 9600);

            if (arduino->isConnected())
                std::cout << "Conexão serial estabelecida!" << std::endl;
        } catch (const SerialException &e) {
            std::cerr << "Erro ao conectar com Arduino: " << e.what() << std::endl;
            std::cerr << "Voltando para controle por teclado..." << std::endl;
            useSerial = false;
        }
    }

public:
    explicit GameScreen(const bool useSerial = false)
        : useSerial(useSerial) {
        initializeSerial();
        startNewMatch();
    }

    void startNewMatch() {
        this->map_manager = std::make_shared<MapManager>();

        this->map_manager->addLayer(asset("mapa_fundo_grama.csv"));
        this->map_manager->addLayer(asset("mapa_borda_floresta.csv"));
        this->map_manager->addLayer(asset("mapa_caminho.csv"));
        this->map_manager->addLayer(asset("mapa_nascente_agua.csv"));
        this->map_manager->addLayer(asset("mapa_pedras_arbustos.csv"));
        this->map_manager->addLayer(asset("mapa_agua.csv"));
        this->map_manager->addLayer(asset("mapa_casas.csv"));
        this->map_manager->addLayer(asset("mapa_tesouros.csv"));

        this->bluePlayer = std::make_shared<Player>(
            BLUE,
            al_load_bitmap(asset("player1.png")),
            MovimentKeyMap{
                ALLEGRO_KEY_W, ALLEGRO_KEY_S, ALLEGRO_KEY_A, ALLEGRO_KEY_D,
                'W', 'S', 'A', 'D'
            },
            8, 3);
        this->bluePlayer->injectMapManager(map_manager);

        this->pinkPlayer = std::make_shared<Player>(
            PINK,
            al_load_bitmap(asset("player2.png")),
            MovimentKeyMap{
                ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
                'T', 'B', 'L', 'R' // serial (Top, Bottom, Left, Right)
            },
            41, 3);
        this->pinkPlayer->injectMapManager(map_manager);

        this->map_manager->updateScore();
    }

    void checkMatchEndCondition() {
        if (map_manager->getEmptySlotsCount() == 0) {
            int blueScore = std::stoi(map_manager->getScore(BLUE));
            int pinkScore = std::stoi(map_manager->getScore(PINK));

            /*
            if (blueScore > pinkScore) {
                winner = BLUE;
            } else if (pinkScore > blueScore) {
                winner = PINK;
            } else {
                winner = std::nullopt;
            }*/
        }
    }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) override {
        if (al_key_down(key_state, ALLEGRO_KEY_R))
            this->startNewMatch();

        // Reseta comandos seriais antes de ler novos
        bluePlayer->setSerialCommand('0');
        pinkPlayer->setSerialCommand('0');

        // Lê comandos do Arduino se disponível
        if (useSerial && arduino && arduino->isConnected()) {
            std::string command = arduino->readLine();
            if (command.length() >= 2) {
                char p1_action = command[0]; // Primeiro caractere para player1
                char p2_action = command[1]; // Segundo caractere para player2

                // Envia comandos para os players
                bluePlayer->setSerialCommand(p1_action);
                pinkPlayer->setSerialCommand(p2_action);

                // std::cout << "Arduino: P1=" << p1_action << " P2=" << p2_action << std::endl;
            }
        }

        bluePlayer->update(event, key_state);
        pinkPlayer->update(event, key_state);

        checkMatchEndCondition();
    }

    void draw() override {
        map_manager->draw();
        bluePlayer->draw();
        pinkPlayer->draw();

        if (useSerial && arduino && arduino->isConnected()) {
            std::string command = arduino->readLine();

            // Por algum motivo desenhar na tela melhora a leitura da porta serial
            drawText(command.c_str(), -10, -10);
        }
    }
};

#endif //TRABALHO_TILEMAP_GAME_SCREEN_H
