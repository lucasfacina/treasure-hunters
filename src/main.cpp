// --- CONFIGURAÇÕES GERAIS ---
#include <cstdio>
#include <memory>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/system.h>

#include "game_manager.h"

#define MAP_WIDTH       50
#define MAP_HEIGHT      30
#define TILE_SIZE       16
#define SCREEN_WIDTH    (TILE_SIZE * MAP_WIDTH)
#define SCREEN_HEIGHT   (TILE_SIZE * MAP_HEIGHT)
#define FPS             60

using namespace std;

ALLEGRO_DISPLAY *display = nullptr;
ALLEGRO_EVENT_QUEUE *event_queue = nullptr;
ALLEGRO_TIMER *timer = nullptr;
ALLEGRO_EVENT event;

shared_ptr<GameManager> game_manager;

int initAllegro() {
    if (!al_init()) {
        printf("Falha ao inicializar Allegro.\n");
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

    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        printf("Falha ao criar display.\n");
        return false;
    }

    event_queue = al_create_event_queue();
    if (!event_queue) {
        printf("Falha ao criar event queue.\n");
        return false;
    }

    timer = al_create_timer(1.0 / FPS);
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
    bool stopRunning = false;
    bool shouldRender = true;
    game_manager = make_shared<GameManager>(MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);
    ALLEGRO_KEYBOARD_STATE key_state;

    while (!stopRunning) {
        if (shouldRender && al_is_event_queue_empty(event_queue)) {
            shouldRender = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
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
}

int main(int argc, char **argv) {
    if (!initAllegro()) return -1;
    registerEventSources();
    updateAndDraw();
    destroyPointers();
    return 0;
}
