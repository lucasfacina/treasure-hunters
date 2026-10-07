/**
 * TRABALHO REALIZADO POR THIAGO AUSECHI (RA:2411600623) E LUCAS FACINA (RA: 2411600015),
 * O TEMA ESCOLHIDO FOI CAÇA AOS TESOUROS EM UMA PEQUENA SELVA COM OBSTÁCULOS. DE TAL MODO,
 * FOI REALIZADA A CONSTRUÇÃO DO MAPA NO TILEMAP, COM O OBJETIVO DE ESTRUTURAR A GAMEPLAY,
 * SENDO 2 PERSONAGENS JOGADORES, QUE DEVEM COLETAR O MÁXIMO DE TESOUROS POSSÍVEL E GUARDAR
 * NOS RESPECTIVOS BAÚS, SENDO QUE, O JOGO TERMINA QUANDO UM DOS JOGADORES ENCHER O DEPÓSITO.
 */

#include <memory>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/system.h>

#include "utils/settings.h"
#include "managers/game_manager.h"

using namespace std;

ALLEGRO_MONITOR_INFO monitor_info;
ALLEGRO_DISPLAY *display = nullptr;
ALLEGRO_EVENT_QUEUE *event_queue = nullptr;
ALLEGRO_TIMER *timer = nullptr;
ALLEGRO_EVENT event;

shared_ptr<GameManager> game_manager;

// Deslocamento usado para centralizar o mapa na tela (faixas pretas nas laterais
// ou em cima/embaixo quando a proporção do monitor é diferente da do mapa)
float OFFSET_X = 0;
float OFFSET_Y = 0;

void calculateScale(int display_width, int display_height) {
    const float map_width = Settings::MAP_WIDTH * Settings::TILE_SIZE;
    const float map_height = Settings::MAP_HEIGHT * Settings::TILE_SIZE;

    // Maior escala possível em que o mapa inteiro ainda cabe na tela, sem distorcer
    Settings::SCALE = std::min(display_width / map_width, display_height / map_height);

    // Centraliza o mapa
    OFFSET_X = (display_width - map_width * Settings::SCALE) / 2.0f;
    OFFSET_Y = (display_height - map_height * Settings::SCALE) / 2.0f;

    Settings::SCREEN_WIDTH = display_width;
    Settings::SCREEN_HEIGHT = display_height;
}

bool initAllegro() {
    if (!al_init()) {
        printf("Falha ao inicializar Allegro.\n");
        return false;
    }
    if (!al_get_monitor_info(0, &monitor_info)) {
        printf("Falha ao obter informações do monitor.\n");
        return false;
    }
    if (!al_install_keyboard()) {
        printf("Falha ao instalar teclado.\n");
        return false;
    }
    if (!al_init_image_addon()) {
        printf("Falha ao inicializar image addon.\n");
        return false;
    }
    if (!al_init_primitives_addon()) {
        printf("Falha ao inicializar primitives addon.\n");
        return false;
    }

    // Tela cheia sem bordas, na resolução nativa do monitor
    int monitor_width = monitor_info.x2 - monitor_info.x1;
    int monitor_height = monitor_info.y2 - monitor_info.y1;
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    display = al_create_display(monitor_width, monitor_height);
    if (!display) {
        printf("Falha ao criar display.\n");
        return false;
    }
    calculateScale(al_get_display_width(display), al_get_display_height(display));
    al_hide_mouse_cursor(display);

    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        printf("Falha ao inicializar o addon de ttf.\n");
        return false;
    }

    Settings::font = al_load_font(asset("PressStart2P.ttf"), 8, 0);
    if (!Settings::font) {
        std::cerr << "Falha ao carregar a fonte 'PressStart2P.ttf'." << std::endl;
        al_destroy_display(display);
        return false;
    }

    event_queue = al_create_event_queue();
    if (!event_queue) {
        printf("Falha ao criar event queue.\n");
        return false;
    }

    timer = al_create_timer(1.0 / 14.0);
    al_start_timer(timer);
    if (!timer) {
        printf("Falha ao criar timer.\n");
        return false;
    }

    return true; // Inicialização bem-sucedida
}

void registerEventSources() {
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
}

void updateAndDraw() {
    Settings::spritesheet = al_load_bitmap(asset("spritesheet.png"));
    Settings::TILE_COLS = al_get_bitmap_width(Settings::spritesheet) / Settings::TILE_SIZE;

    bool stopRunning = false;
    bool shouldRender = true;
    game_manager = make_shared<GameManager>();
    game_manager->init();
    ALLEGRO_KEYBOARD_STATE key_state;
    ALLEGRO_TRANSFORM transform;

    while (!stopRunning) {
        if (shouldRender && al_is_event_queue_empty(event_queue)) {
            shouldRender = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_identity_transform(&transform);
            al_scale_transform(&transform, Settings::SCALE, Settings::SCALE);
            al_translate_transform(&transform, OFFSET_X, OFFSET_Y);
            al_use_transform(&transform);
            game_manager->draw();
            al_flip_display();
        }

        al_wait_for_event(event_queue, &event);
        al_get_keyboard_state(&key_state);

        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                stopRunning = true;
                break;
            case ALLEGRO_EVENT_TIMER:
                stopRunning = al_key_down(&key_state, ALLEGRO_KEY_ESCAPE);
                game_manager->update(&event, &key_state);
                shouldRender = true;
                break;
            default:
                break;
        }
    }
}

void destroyPointers() {
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_bitmap(Settings::spritesheet);
}

int main(int argc, char **argv) {
    if (!initAllegro()) return -1;
    registerEventSources();
    updateAndDraw();
    destroyPointers();
    return 0;
}
