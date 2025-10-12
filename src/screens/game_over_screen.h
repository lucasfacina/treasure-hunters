#ifndef TRABALHO_TILEMAP_GAME_OVER_H
#define TRABALHO_TILEMAP_GAME_OVER_H

#include <utility>

#include "screen.h"
#include "game_over_info.h"
#include "utils/darken_background.h"
#include "utils/draw_text.h"
#include "utils/format_time.h"


class GameOverScreen final : public Screen {
    const GameOverInfo info;

public:
    explicit GameOverScreen(GameOverInfo info) : info(std::move(info)) {}

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) override;

    void draw() override {
        darkenBackground();

        const auto centerWidth = Settings::MAP_WIDTH / 2;
        const auto screenPortion = Settings::MAP_HEIGHT / 10;

        drawText(
            "Fim de Jogo",
            centerWidth,
            screenPortion,
            al_map_rgb(255, 0, 0),
            al_map_rgb(128, 0, 0)
        );

        auto winnerText = "Empate!";

        if (info.blueScore > info.pinkScore)
            winnerText = "Azul venceu!";
        else if (info.pinkScore > info.blueScore)
            winnerText = "Rosa venceu!";

        drawText(winnerText, centerWidth, screenPortion * 5);

        drawText(
            std::format("A partida durou {}", formatTime(info.matchDurationInSeconds)).c_str(),
            centerWidth, screenPortion * 8
        );

        // TODO: Aguardar resposta do envio de dados para o servidor
        auto restartText = "Pressione [ENTER] para reiniciar";
        drawText(restartText, centerWidth, screenPortion * 9);
    }

    void init() override {}

    void onFocus() override {}

    void onLoseFocus() override {}

    void onExit() override {}
};

#endif //TRABALHO_TILEMAP_GAME_OVER_H
