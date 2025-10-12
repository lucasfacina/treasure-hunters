#ifndef TRABALHO_TILEMAP_DARKEN_BACKGROUND_H
#define TRABALHO_TILEMAP_DARKEN_BACKGROUND_H
#include <allegro5/allegro_primitives.h>

#include "settings.h"

inline void darkenBackground(const float darknessPercentage = 85) {
    const auto alpha = static_cast<unsigned char>((darknessPercentage / 100.0f) * 255.0f);

    al_draw_filled_rectangle(
        0, 0,
        Settings::MAP_WIDTH * Settings::TILE_SIZE,
        Settings::MAP_HEIGHT * Settings::TILE_SIZE,
        al_map_rgba(0, 0, 0, alpha)
    );
}

#endif //TRABALHO_TILEMAP_DARKEN_BACKGROUND_H
