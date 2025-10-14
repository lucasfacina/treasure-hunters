#ifndef TRABALHO_TILEMAP_GAME_OVER_H
#define TRABALHO_TILEMAP_GAME_OVER_H

#include <utility>
#include <future>

#include "screen.h"
#include "game_over_info.h"
#include "api/api_client.h"
#include "utils/darken_background.h"
#include "utils/draw_text.h"
#include "utils/format_time.h"

class GameOverScreen final : public Screen {
    const GameOverInfo info;

    std::unique_ptr<ApiClient> apiClient;
    std::optional<std::future<ApiResponse>> asyncSubmissionResult;
    SubmissionState submissionState;

    std::string serverResponseMessage;
    std::string finalScoreText;
    std::string winnerText;
    std::string matchDurationText;
    std::string pressKeyToRestartText;

    std::string formatScore(const int score) {
        if (score == 1) return "1 ponto";
        return std::format("{} pontos", score);
    }

public:
    explicit GameOverScreen(GameOverInfo info) : info(std::move(info)) {}

    void update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) override;

    void draw() override {
        darkenBackground();

        const auto centerWidth = Settings::MAP_WIDTH / 2;
        const auto screenPortion = Settings::MAP_HEIGHT / 10;

        drawText("Fim de Jogo!",
                 centerWidth, screenPortion,
                 al_map_rgb(255, 0, 0), al_map_rgb(110, 0, 0)
        );
        drawText(this->serverResponseMessage.c_str(), centerWidth, screenPortion * 2);
        drawText(this->winnerText.c_str(),
                 centerWidth, screenPortion * 5,
                 al_map_rgb(227, 180, 68), al_map_rgb(33, 32, 46)
        );
        drawText(this->finalScoreText.c_str(), centerWidth, 1 + screenPortion * 5);
        drawText(this->matchDurationText.c_str(), centerWidth, screenPortion * 8);
        drawText(this->pressKeyToRestartText.c_str(), centerWidth, screenPortion * 9);
    }

    void init() override {
        this->finalScoreText = std::format("Azul ({}) vs Rosa ({})", this->info.blueScore, this->info.pinkScore);
        this->matchDurationText = std::format("A partida durou {}", formatTime(info.matchDurationInSeconds));

        this->winnerText = "Empate!";
        int scoreAbsoluteDifference = std::abs(this->info.blueScore - this->info.pinkScore);
        if (this->info.blueScore > this->info.pinkScore)
            this->winnerText = "Azul";
        else
            this->winnerText = "Rosa";
        if (scoreAbsoluteDifference != 0)
            this->winnerText = std::format(
                "{} venceu com {} de vantagem!", this->winnerText, formatScore(scoreAbsoluteDifference)
            );

        this->submissionState = SubmissionState::SENDING;
        this->serverResponseMessage = "";
        this->pressKeyToRestartText = "Enviando resultados da partida...";
        this->apiClient = std::make_unique<ApiClient>();

        // Usamos uma lambda para mover o unique_ptr do cliente para a nova thread,
        // garantindo que ele viva o tempo necessário.
        this->asyncSubmissionResult = std::async(
            std::launch::async,
            [client = std::move(this->apiClient), info = this->info]() {
                return client->submitGameResults(info);
            }
        );
    }

    void onFocus() override {}

    void onLoseFocus() override {}

    void onExit() override {}
};

#endif //TRABALHO_TILEMAP_GAME_OVER_H
