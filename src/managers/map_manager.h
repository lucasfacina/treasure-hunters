#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
#include <vector>

#include "../utils/layer.h"
#include "objects/game_object.h"
#include "utils/object_factory.h"
#include "utils/position.h"

class MapManager final : public std::enable_shared_from_this<MapManager> {
    std::vector<std::shared_ptr<Layer>> layers;

    // Estrutura para iteração rápida sobre todos os objetos
    std::vector<std::shared_ptr<GameObject>> allGameObjects;
    // Estrutura para busca rápida por posição
    std::unordered_map<Position, std::vector<std::shared_ptr<GameObject>>, PositionHash> gameObjectsByPosition;

    void loadLayer(const std::shared_ptr<Layer> &layer, const char *filename) {
        FILE *file = fopen(filename, "r");
        if (!file) {
            printf("Erro ao abrir o arquivo de assets: %s\n", filename);
            return;
        }

        char line[4096];
        int row = 0;
        while (fgets(line, sizeof(line), file) && row < Settings::MAP_HEIGHT) {
            int col = 0;
            const char *token = strtok(line, ",");
            while (token != nullptr && col < Settings::MAP_WIDTH) {
                const int tileIndex = atoi(token);

                auto gameObj = createById(tileIndex, col, row);

                if (gameObj != nullptr) {
                    this->addGameObject(gameObj);

                    if (!gameObj->isSelfRender())
                        layer->setTileAt(col, row, tileIndex);
                } else {
                    layer->setTileAt(col, row, tileIndex);
                }

                token = strtok(nullptr, ",");
                col++;
            }
            row++;
        }
        fclose(file);
    }

public:
    ~MapManager() {
        this->layers.clear();
    }

    void addLayer(const char *csv_map_file) {
        auto new_layer = std::make_shared<Layer>();

        this->loadLayer(new_layer, csv_map_file);

        this->layers.push_back(new_layer);
    }

    void addGameObject(const std::shared_ptr<GameObject> &gameObject) {
        gameObject->injectMapManager(shared_from_this());

        Position pos = {gameObject->getX(), gameObject->getY()};
        gameObjectsByPosition[pos].push_back(gameObject);
        allGameObjects.push_back(gameObject);
    }

    void removeGameObject(const std::shared_ptr<GameObject> &gameObject) {
        Position pos = {gameObject->getX(), gameObject->getY()};
        auto map_it = gameObjectsByPosition.find(pos);
        if (map_it != gameObjectsByPosition.end()) {
            auto &vec = map_it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), gameObject), vec.end());
            if (vec.empty())
                gameObjectsByPosition.erase(map_it);
        }

        allGameObjects.erase(
            std::remove(allGameObjects.begin(), allGameObjects.end(), gameObject),
            allGameObjects.end()
        );
    }

    std::vector<std::shared_ptr<GameObject>> findGameObjectsAt(int x, int y) {
        auto gameObjectsAtThePosition = gameObjectsByPosition.find({x, y});
        if (gameObjectsAtThePosition != gameObjectsByPosition.end())
            return gameObjectsAtThePosition->second;

        return {}; // Retorna um vetor vazio se não encontrar objetos
    }

    void moveGameObject(const std::shared_ptr<GameObject> &gameObject, int newX, int newY) {
        std::cout << "Movendo para: (" << newX << ", " << newY << ")" << std::endl;

        Position oldPos = {gameObject->getX(), gameObject->getY()};

        // Remover o ponteiro do GameObject da lista da posição antiga
        auto it = gameObjectsByPosition.find(oldPos);
        if (it != gameObjectsByPosition.end()) {
            auto &objects_at_old_pos = it->second;

            objects_at_old_pos.erase(
                std::remove(objects_at_old_pos.begin(), objects_at_old_pos.end(), gameObject),
                objects_at_old_pos.end()
            );

            if (objects_at_old_pos.empty())
                gameObjectsByPosition.erase(it);
        }

        gameObject->setPosition(newX, newY);

        Position newPos = {newX, newY};
        gameObjectsByPosition[newPos].push_back(gameObject);
    }

    const std::vector<std::shared_ptr<GameObject>> &getAllGameObjects() const {
        return allGameObjects;
    }

    void draw() const {
        for (const auto &layer: this->layers)
            layer->draw();

        for (const auto &gameObject: this->allGameObjects)
            gameObject->draw();
    }
};

#endif //MAP_MANAGER_H
