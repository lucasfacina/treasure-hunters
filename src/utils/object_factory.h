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
        case 278: return std::make_shared<TreasureObject>("BronzeCoin", id, 10, x, y);
        case 279: return std::make_shared<TreasureObject>("BronzeSmallPile", id, 20, x, y);
        case 280: return std::make_shared<TreasureObject>("BronzeLargePile", id, 30, x, y);
        case 281: return std::make_shared<TreasureObject>("BronzeBag", id, 40, x, y);
        case 282: return std::make_shared<TreasureObject>("BronzeChalice", id, 50, x, y);
        case 283: return std::make_shared<TreasureObject>("BronzeIngot", id, 60, x, y);

        // Tesouros de Prata
        case 295: return std::make_shared<TreasureObject>("SilverCoin", id, 15, x, y);
        case 296: return std::make_shared<TreasureObject>("SilverSmallPile", id, 25, x, y);
        case 297: return std::make_shared<TreasureObject>("SilverLargePile", id, 35, x, y);
        case 298: return std::make_shared<TreasureObject>("SilverBag", id, 45, x, y);
        case 299: return std::make_shared<TreasureObject>("SilverChalice", id, 55, x, y);
        case 300: return std::make_shared<TreasureObject>("SilverIngot", id, 65, x, y);

        // Tesouros de Ouro
        case 312: return std::make_shared<TreasureObject>("GoldenCoin", id, 20, x, y);
        case 313: return std::make_shared<TreasureObject>("GoldenSmallPile", id, 30, x, y);
        case 314: return std::make_shared<TreasureObject>("GoldenLargePile", id, 40, x, y);
        case 315: return std::make_shared<TreasureObject>("GoldenBag", id, 50, x, y);
        case 316: return std::make_shared<TreasureObject>("GoldenChalice", id, 60, x, y);
        case 317: return std::make_shared<TreasureObject>("GoldenIngot", id, 70, x, y);
        case 318: return std::make_shared<TreasureObject>("GoldenClover", id, 100, x, y);

        default: return nullptr;
    }
}

#endif //OBJECT_FACTORY_H
