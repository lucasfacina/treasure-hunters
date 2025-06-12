// --- CONFIGURAÇÕES GERAIS ---
#include <memory>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
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

void calculateScreenSize(int &screen_width, int &screen_height) {
    // Largura e altura DISPONÍVEL do monitor
    int monitor_width = monitor_info.x2 - monitor_info.x1;
    int monitor_height = monitor_info.y2 - monitor_info.y1;

    // Calcula o fator de escala baseado na LARGURA DISPONÍVEL
    // Quanto do monitor a largura do nosso mapa (em pixels) ocuparia
    float scale_by_width = static_cast<float>(monitor_width) / (Settings::MAP_WIDTH * Settings::TILE_SIZE);

    // Calcula o fator de escala baseado na ALTURA DISPONÍVEL
    // Quanto do monitor a altura do nosso mapa (em pixels) ocuparia
    float scale_by_height = static_cast<float>(monitor_height) / (Settings::MAP_HEIGHT * Settings::TILE_SIZE);

    // Escolhe o MENOR fator de escala para garantir que o mapa CAIBA no monitor
    Settings::SCALE = std::min(scale_by_width, scale_by_height);

    // Reduz o SCALE se não for 1 para evitar que o mapa fique muito grande
    Settings::SCALE *= Settings::SCALE != 1
                           ? 0.9F
                           : 1.0f;

    // Calcula as dimensões finais do display usando o SCALE final
    screen_width = static_cast<int>(Settings::MAP_WIDTH * Settings::TILE_SIZE * Settings::SCALE);
    screen_height = static_cast<int>(Settings::MAP_HEIGHT * Settings::TILE_SIZE * Settings::SCALE);
}

int initAllegro() {
    if (!al_init()) {
        printf("Falha ao inicializar Allegro.\n");
        return false;
    }
    if (!al_get_monitor_info(0, &monitor_info)) {
        // 0 geralmente é o monitor principal
        printf("Falha ao obter informações do monitor.\n");
        return -1;
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

    int screen_width, screen_height;
    calculateScreenSize(screen_width, screen_height);
    display = al_create_display(screen_width, screen_height);
    if (!display) {
        printf("Falha ao criar display.\n");
        return false;
    }

    event_queue = al_create_event_queue();
    if (!event_queue) {
        printf("Falha ao criar event queue.\n");
        return false;
    }

    timer = al_create_timer(1.0 / 15.0);
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
    ALLEGRO_KEYBOARD_STATE key_state;
    ALLEGRO_TRANSFORM transform;

    while (!stopRunning) {
        if (shouldRender && al_is_event_queue_empty(event_queue)) {
            shouldRender = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_identity_transform(&transform);
            al_scale_transform(&transform, Settings::SCALE, Settings::SCALE);
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
