#ifndef SPRITES_H
#define SPRITES_H
#include <allegro5/bitmap.h>

namespace Settings {
   inline const int MAP_WIDTH = 50;
   inline const int MAP_HEIGHT = 30;

   inline const int TILE_SIZE = 16;
   inline int TILE_COLS = 0;
   inline ALLEGRO_BITMAP *spritesheet = nullptr;
}

#endif //SPRITES_H
