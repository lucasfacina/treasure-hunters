#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H
#include <memory>

#include "../objects/chest.h"
#include "../objects/collidable.h"
#include "../objects/game_object.h"
#include "../objects/treasure.h"
#include "objects/house_slot.h"

inline std::shared_ptr<GameObject> createById(const int id, const int x, const int y) {
    const bool floresta_borda = (id >= 0 && id <= 4) ||
                                (id >= 17 && id <= 21) ||
                                (id >= 34 && id <= 38) ||
                                (id >= 51 && id <= 55) ||
                                (id >= 68 && id <= 72);

    const bool decoracao = (id >= 143 && id <= 148) ||
                           (id >= 160 && id <= 165) ||
                           (id >= 177 && id <= 182) ||
                           (id >= 194 && id <= 199) ||
                           id == 25 || id == 32;

    const bool casa = (id >= 7 && id <= 13) || (id >= 58 && id <= 64);

    const bool agua = (id >= 255 && id <= 260) ||
                      (id >= 272 && id <= 277) ||
                      (id >= 289 && id <= 292) ||
                      (id >= 306 && id <= 309);

    if (floresta_borda || casa || decoracao || agua)
        return std::make_shared<CollidableObject>(x, y);

    switch (id) {
        case 14: return std::make_shared<HouseSlotObject>(PINK, x, y);
        case 65: return std::make_shared<HouseSlotObject>(BLUE, x, y);

        case 27: return std::make_shared<ChestObject>(PINK, x, y);
        case 44: return std::make_shared<ChestObject>(BLUE, x, y);

        // Tesouros de Bronze
        case 278: return std::make_shared<TreasureObject>(id, 10, x, y); // Moeda
        case 279: return std::make_shared<TreasureObject>(id, 20, x, y); // Pilha Pequena Moeda
        case 280: return std::make_shared<TreasureObject>(id, 30, x, y); // Pilha Grande Moeda
        case 281: return std::make_shared<TreasureObject>(id, 40, x, y); // Saco Moeda
        case 282: return std::make_shared<TreasureObject>(id, 50, x, y); // Cálice
        case 283: return std::make_shared<TreasureObject>(id, 60, x, y); // Barra

        // Tesouros de Prata
        case 295: return std::make_shared<TreasureObject>(id, 15, x, y); // Moeda
        case 296: return std::make_shared<TreasureObject>(id, 25, x, y); // Pilha Pequena Moeda
        case 297: return std::make_shared<TreasureObject>(id, 35, x, y); // Pilha Grande Moeda
        case 298: return std::make_shared<TreasureObject>(id, 45, x, y); // Saco Moeda
        case 299: return std::make_shared<TreasureObject>(id, 55, x, y); // Cálice
        case 300: return std::make_shared<TreasureObject>(id, 65, x, y); // Barra

        // Tesouros de Ouro
        case 312: return std::make_shared<TreasureObject>(id, 20, x, y); // Moeda
        case 313: return std::make_shared<TreasureObject>(id, 30, x, y); // Pilha Pequena Moeda
        case 314: return std::make_shared<TreasureObject>(id, 40, x, y); // Pilha Grande Moeda
        case 315: return std::make_shared<TreasureObject>(id, 50, x, y); // Saco Moeda
        case 316: return std::make_shared<TreasureObject>(id, 60, x, y); // Cálice
        case 317: return std::make_shared<TreasureObject>(id, 70, x, y); // Barra
        case 318: return std::make_shared<TreasureObject>(id, 100, x, y); // Trevo

        default: return nullptr;
    }
}

#endif //OBJECT_FACTORY_H
