#include <cstdio>
#include <cstring>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

// --- CONFIGURAÇÕES GERAIS ---
#define TILE_WIDTH      16
#define TILE_HEIGHT     16
#define MAP_WIDTH       50
#define MAP_HEIGHT      30
#define PLAYER_WIDTH    16
#define PLAYER_HEIGHT   16
#define SCREEN_WIDTH    (TILE_WIDTH * MAP_WIDTH)
#define SCREEN_HEIGHT   (TILE_HEIGHT * MAP_HEIGHT)
#define NUM_CENARIO_LAYERS 6
#define NUM_ITEM_LAYERS    2

// Teclas para os jogadores
enum KEYS_P1 { UP, DOWN, LEFT, RIGHT };

enum KEYS_P2 { W, S, A, D };

// --- VARIÁVEIS GLOBAIS ---
ALLEGRO_BITMAP *cenario_tileset = nullptr;
ALLEGRO_BITMAP *itens_tileset = nullptr;
ALLEGRO_BITMAP *player_sprite = nullptr;

// Arrays para armazenar todas as camadas do assets
int cenario_layers[NUM_CENARIO_LAYERS][MAP_HEIGHT][MAP_WIDTH];
int item_layers[NUM_ITEM_LAYERS][MAP_HEIGHT][MAP_WIDTH];
int collision_layer[MAP_HEIGHT][MAP_WIDTH];

int player1_inventory_count = 0;
int player2_inventory_count = 0;


// --- DEFINIÇÃO DAS FUNÇÕES DE AJUDA ---

void load_map_from_csv(const char *filename, int target_map[MAP_HEIGHT][MAP_WIDTH]) {
        FILE *file   = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de assets: %s\n", filename);
        return;
    }
    char line[4096];
    int row = 0;
    while (fgets(line, sizeof(line), file) && row < MAP_HEIGHT) {
        int col = 0;
        char *token = strtok(line, ",");
        while (token != nullptr && col < MAP_WIDTH) {
            target_map[row][col] = atoi(token);
            token = strtok(nullptr, ",");
            col++;
        }
        row++;
    }
    fclose(file);
}

void draw_layer(int layer_map[MAP_HEIGHT][MAP_WIDTH], ALLEGRO_BITMAP *current_tileset) {
    if (!current_tileset) return;
    int tileset_cols = al_get_bitmap_width(current_tileset) / TILE_WIDTH;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            int tile_index = layer_map[i][j];
            if (tile_index != -1) {
                int sx = (tile_index % tileset_cols) * TILE_WIDTH;
                int sy = (tile_index / tileset_cols) * TILE_HEIGHT;
                int dx = j * TILE_WIDTH;
                int dy = i * TILE_HEIGHT;
                al_draw_bitmap_region(current_tileset, sx, sy, TILE_WIDTH, TILE_HEIGHT, dx, dy, 0);
            }
        }
    }
}

void draw_all_layers() {
    // Desenha camadas de cenário com o tileset de cenário
    for (int i = 0; i < NUM_CENARIO_LAYERS; i++) {
        draw_layer(cenario_layers[i], cenario_tileset);
    }
    // Desenha camadas de itens com o tileset de itens
    for (int i = 0; i < NUM_ITEM_LAYERS; i++) {
        draw_layer(item_layers[i], itens_tileset);
    }
}

bool check_collision(int x, int y) {
    int corners[4][2] = {
        {x, y}, {x + PLAYER_WIDTH - 1, y}, {x, y + PLAYER_HEIGHT - 1}, {x + PLAYER_WIDTH - 1, y + PLAYER_HEIGHT - 1}
    };
    for (int i = 0; i < 4; i++) {
        int col = corners[i][0] / TILE_WIDTH;
        int row = corners[i][1] / TILE_HEIGHT;
        if (row >= 0 && row < MAP_HEIGHT && col >= 0 && col < MAP_WIDTH && collision_layer[row][col] != -1) {
            return true;
        }
    }
    return false;
}

void add_item_to_inventory(int item_tile_index, int player_id) {
    if (player_id == 1) {
        int inv_col = player1_inventory_count;
        if (inv_col < MAP_WIDTH) {
            item_layers[0][0][inv_col] = item_tile_index;
            player1_inventory_count++;
        }
    } else if (player_id == 2) {
        int inv_col = player2_inventory_count;
        if (inv_col < MAP_WIDTH) {
            item_layers[0][1][inv_col] = item_tile_index;
            player2_inventory_count++;
        }
    }
}

void check_item_collection(int player_x, int player_y, int player_id) {
    int col = (player_x + PLAYER_WIDTH / 2) / TILE_WIDTH;
    int row = (player_y + PLAYER_HEIGHT / 2) / TILE_HEIGHT;
    if (row >= 0 && row < MAP_HEIGHT && col >= 0 && col < MAP_WIDTH) {
        // Lógica atual só coleta da primeira camada de item.
        if (item_layers[0][row][col] != -1) {
            int collected_item_index = item_layers[0][row][col];
            item_layers[0][row][col] = -1;
            add_item_to_inventory(collected_item_index, player_id);
        }
    }
}

//======================================================================
// --- FUNÇÃO PRINCIPAL (main) ---
//======================================================================
int _main() {
    // --- INICIALIZAÇÃO DO ALLEGRO ---
    if (!al_init()) {
        printf("Falha ao inicializar Allegro.\n");
        return -1;
    }
    if (!al_install_keyboard()) {
        printf("Falha ao instalar teclado.\n");
        return -1;
    }
    if (!al_init_image_addon()) {
        printf("Falha ao inicializar image addon.\n");
        return -1;
    }
    if (!al_init_primitives_addon()) {
        printf("Falha ao inicializar primitives addon.\n");
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        printf("Falha ao criar display.\n");
        return -1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        printf("Falha ao criar timer.\n");
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    if (!event_queue) {
        printf("Falha ao criar event queue.\n");
        return -1;
    }

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // --- CARREGAMENTO DOS RECURSOS ---

    // !!! AJUSTE OBRIGATÓRIO: COLOQUE O CAMINHO EXATO DA SUA PASTA 'src' AQUI !!!
    const char *path_prefix = "C:\\Users\\GalaxyBook3 de Lucas\\CLionProjects\\trabalho_tilemap\\src\\";
    char full_path[1024];

    sprintf(full_path, "%smapa\\cenario.png", path_prefix);
    cenario_tileset = al_load_bitmap(full_path);
    if (!cenario_tileset) {
        printf("Erro ao carregar tileset: %s\n", full_path);
        return -1;
    }

    sprintf(full_path, "%smapa\\tesouro.png", path_prefix);
    itens_tileset = al_load_bitmap(full_path);
    if (!itens_tileset) {
        printf("Erro ao carregar tileset de itens: %s\n", full_path);
        return -1;
    }

    sprintf(full_path, "%smapa\\personagem.png", path_prefix);
    player_sprite = al_load_bitmap(full_path);
    if (!player_sprite) {
        printf("Erro ao carregar sprite do jogador: %s\n", full_path);
        return -1;
    }

    // Lista de arquivos de cenário na ordem correta de desenho (fundo primeiro)
    const char *cenario_files[NUM_CENARIO_LAYERS] = {
        //"mapa_Grama.csv",
        //"mapa_Agua.csv",
        //"mapa_Cenario_Enfeite.csv",
        //"mapa_Arvores.csv",
        //"mapa_Arbustos.csv",
        //"mapa_.Tesouro.csv"
    };

    // for (int i = 0; i < NUM_CENARIO_LAYERS; i++) {
        // sprintf(full_path, "%smapa\\%s", path_prefix, cenario_files[i]);
        // load_map_from_csv(full_path, cenario_layers[i]);
    // }

    // sprintf(full_path, "%smapa\\mapa_Caminho.csv", path_prefix);
    // load_map_from_csv(full_path, collision_layer);

    // sprintf(full_path, "%smapa\\mapa_Tesouro.csv", path_prefix);
    // load_map_from_csv(full_path, item_layers[0]);

    // --- VARIÁVEIS DO JOGO ---
    bool done = false;
    bool redraw = true;
    int p1_x = 100, p1_y = 100;
    int p2_x = SCREEN_WIDTH - 150, p2_y = SCREEN_HEIGHT - 150;
    int move_speed = 2;
    bool keys_p1[4] = {false}, keys_p2[4] = {false};

    al_start_timer(timer);

    // --- LOOP PRINCIPAL DO JOGO ---
    while (!done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (keys_p1[UP] && !check_collision(p1_x, p1_y - move_speed)) p1_y -= move_speed;
            if (keys_p1[DOWN] && !check_collision(p1_x, p1_y + move_speed)) p1_y += move_speed;
            if (keys_p1[LEFT] && !check_collision(p1_x - move_speed, p1_y)) p1_x -= move_speed;
            if (keys_p1[RIGHT] && !check_collision(p1_x + move_speed, p1_y)) p1_x += move_speed;

            if (keys_p2[W] && !check_collision(p2_x, p2_y - move_speed)) p2_y -= move_speed;
            if (keys_p2[S] && !check_collision(p2_x, p2_y + move_speed)) p2_y += move_speed;
            if (keys_p2[A] && !check_collision(p2_x - move_speed, p2_y)) p2_x -= move_speed;
            if (keys_p2[D] && !check_collision(p2_x + move_speed, p2_y)) p2_x += move_speed;

            check_item_collection(p1_x, p1_y, 1);
            check_item_collection(p2_x, p2_y, 2);
            redraw = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            done = true;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP: keys_p1[UP] = true;
                    break;
                case ALLEGRO_KEY_DOWN: keys_p1[DOWN] = true;
                    break;
                case ALLEGRO_KEY_LEFT: keys_p1[LEFT] = true;
                    break;
                case ALLEGRO_KEY_RIGHT: keys_p1[RIGHT] = true;
                    break;
                case ALLEGRO_KEY_W: keys_p2[W] = true;
                    break;
                case ALLEGRO_KEY_S: keys_p2[S] = true;
                    break;
                case ALLEGRO_KEY_A: keys_p2[A] = true;
                    break;
                case ALLEGRO_KEY_D: keys_p2[D] = true;
                    break;
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP: keys_p1[UP] = false;
                    break;
                case ALLEGRO_KEY_DOWN: keys_p1[DOWN] = false;
                    break;
                case ALLEGRO_KEY_LEFT: keys_p1[LEFT] = false;
                    break;
                case ALLEGRO_KEY_RIGHT: keys_p1[RIGHT] = false;
                    break;
                case ALLEGRO_KEY_W: keys_p2[W] = false;
                    break;
                case ALLEGRO_KEY_S: keys_p2[S] = false;
                    break;
                case ALLEGRO_KEY_A: keys_p2[A] = false;
                    break;
                case ALLEGRO_KEY_D: keys_p2[D] = false;
                    break;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_all_layers();
            al_draw_bitmap(player_sprite, p1_x, p1_y, 0);
            al_draw_bitmap(player_sprite, p2_x, p2_y, 0);
            al_flip_display();
        }
    }

    // --- FINALIZAÇÃO E LIMPEZA ---
    al_destroy_bitmap(cenario_tileset);
    al_destroy_bitmap(itens_tileset);
    al_destroy_bitmap(player_sprite);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    return 0;
}
