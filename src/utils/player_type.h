#ifndef PLAYER_TYPE_H
#define PLAYER_TYPE_H

enum PlayerType {
    BLUE, PINK,

    COUNT // Deve ser o último elemento, usado para contar o número de tipos de jogadores
};

inline std::string playerTypeToString(PlayerType type) {
    switch (type) {
        case BLUE: return "BLUE";
        case PINK: return "PINK";
        default: return "UNKNOWN";
    }
}

#endif //PLAYER_TYPE_H
