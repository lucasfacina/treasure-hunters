#ifndef SETTINGS_H
#define SETTINGS_H
#include <allegro5/bitmap.h>

namespace Settings {
   inline const int MAP_WIDTH = 50;
   inline const int MAP_HEIGHT = 30;

   inline const int TILE_SIZE = 16;
   inline float SCALE = 1;
   inline int TILE_COLS = 0;
   inline ALLEGRO_BITMAP *spritesheet = nullptr;

#ifdef _WIN32
   inline const char *ASSETS_PATH = "assets/";
#else
   inline const char *ASSETS_PATH = "../assets/";
#endif
}

#endif //SETTINGS_H
