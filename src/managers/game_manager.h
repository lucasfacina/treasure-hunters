#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "map_manager.h"
#include "../objects/player.h"
#include "utils/assets.h"
#include "serial_port.h"
#include <iostream>

inline std::string getArduinoPort() {
    // std::string port;
    // do {
    //     std::cout << "Conecte o Arduino e insira a porta: " << std::endl;
    //     std::cout << "\tWindows: geralmente COM3" << std::endl;
    //     std::cout << "\tLinux: geralmente /dev/ttyACM0 ou /dev/ttyUSB0" << std::endl;
    //     std::cout << "\tMacOS: geralmente /dev/tty.usbmodemXXXX" << std::endl;
    //     std::getline(std::cin, port);
    // } while (port.empty());
    // return port;
    return "COM3";
}

class GameManager {
    std::shared_ptr<MapManager> map_manager;
    std::shared_ptr<Player> player1;
    std::shared_ptr<Player> player2;

    std::unique_ptr<SerialPort> arduino;
    bool useSerial;

public:
    explicit GameManager(bool enableSerial = false) : useSerial(enableSerial) {
        this->map_manager = std::make_shared<MapManager>();

        this->map_manager->addLayer(asset("mapa_fundo_grama.csv"));
        this->map_manager->addLayer(asset("mapa_borda_floresta.csv"));
        this->map_manager->addLayer(asset("mapa_caminho.csv"));
        this->map_manager->addLayer(asset("mapa_nascente_agua.csv"));
        this->map_manager->addLayer(asset("mapa_pedras_arbustos.csv"));
        this->map_manager->addLayer(asset("mapa_agua.csv"));
        this->map_manager->addLayer(asset("mapa_casas.csv"));
        this->map_manager->addLayer(asset("mapa_tesouros.csv"));

        if (useSerial) {
            try {
                arduino = std::make_unique<SerialPort>(getArduinoPort(), 9600);
                std::cout << "Conexão serial estabelecida!" << std::endl;
            } catch (const SerialException &e) {
                std::cerr << "Erro ao conectar com Arduino: " << e.what() << std::endl;
                std::cerr << "Voltando para controle por teclado..." << std::endl;
                useSerial = false;
            }
        }

        this->player1 = std::make_shared<Player>(
            BLUE,
            al_load_bitmap(asset("player1.png")),
            MovimentKeyMap{
                ALLEGRO_KEY_W, ALLEGRO_KEY_S, ALLEGRO_KEY_A, ALLEGRO_KEY_D,
                'W', 'S', 'A', 'D'
            },
            8, 3);
        this->player1->injectMapManager(map_manager);

        this->player2 = std::make_shared<Player>(
            PINK,
            al_load_bitmap(asset("player2.png")),
            MovimentKeyMap{
                ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
                'T', 'B', 'L', 'R' // serial (Top, Bottom, Left, Right)
            },
            41, 3);
        this->player2->injectMapManager(map_manager);

        this->map_manager->updateScore();
    }

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) const {
        // Reseta comandos seriais antes de ler novos
        player1->setSerialCommand('0');
        player2->setSerialCommand('0');

        // Lê comandos do Arduino se disponível
        if (useSerial && arduino && arduino->isConnected()) {
            std::string command = arduino->readLine();
            if (command.length() >= 2) {
                char p1_action = command[0]; // Primeiro caractere para player1
                char p2_action = command[1]; // Segundo caractere para player2

                // Envia comandos para os players
                player1->setSerialCommand(p1_action);
                player2->setSerialCommand(p2_action);

                // std::cout << "Arduino: P1=" << p1_action << " P2=" << p2_action << std::endl;
            }
        }

        player1->update(event, key_state);
        player2->update(event, key_state);
    }

    void draw() const {
        map_manager->draw();
        player1->draw();
        player2->draw();
    }
};

#endif // GAME_MANAGER_H
