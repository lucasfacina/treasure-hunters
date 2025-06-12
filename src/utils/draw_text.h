#ifndef DRAW_TEXT_H
#define DRAW_TEXT_H
#include <allegro5/allegro_font.h>

#include "settings.h"

inline void drawText(
    const char *text,
    const float textX,
    const float textY,
    const ALLEGRO_COLOR text_color,
    const ALLEGRO_COLOR border_color) {
    const auto x = textX * Settings::TILE_SIZE;
    const auto y = textY * Settings::TILE_SIZE;
    const auto flag = ALLEGRO_ALIGN_CENTER;

    al_draw_text(Settings::font, border_color, x - 1, y, flag, text);
    al_draw_text(Settings::font, border_color, x + 1, y, flag, text);
    al_draw_text(Settings::font, border_color, x, y - 1, flag, text);
    al_draw_text(Settings::font, border_color, x, y + 1, flag, text);
    al_draw_text(Settings::font, border_color, x - 1, y - 1, flag, text);
    al_draw_text(Settings::font, border_color, x + 1, y - 1, flag, text);
    al_draw_text(Settings::font, border_color, x - 1, y + 1, flag, text);
    al_draw_text(Settings::font, border_color, x + 1, y + 1, flag, text);

    al_draw_text(Settings::font, text_color, x, y, flag, text);
}

inline void drawText(const char *text, const float x, const float y) {
    drawText(text, x, y, al_map_rgb(255, 255, 255), al_map_rgb(0, 0, 0));
}

#endif //DRAW_TEXT_H
