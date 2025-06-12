#ifndef ASSETS_H
#define ASSETS_H
#include <cstring>

#include "settings.h"

inline const char *asset(const char *filename) {
    if (!filename) return nullptr;

    size_t path_len = std::strlen(Settings::ASSETS_PATH);
    size_t file_len = std::strlen(filename);
    size_t total_len = path_len + file_len + 1; // +1 para o '\0'

    char *result = static_cast<char *>(std::malloc(total_len));
    if (!result) return nullptr; // Falha na alocação

    std::strcpy(result, Settings::ASSETS_PATH);
    std::strcat(result, filename);

    return result;
}

#endif //ASSETS_H
