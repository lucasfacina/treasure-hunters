#include <allegro5/events.h>
#include <allegro5/keyboard.h>

#include "managers/game_manager.h"
#include "game_over_screen.h"

void GameOverScreen::update(const ALLEGRO_EVENT *event, const ALLEGRO_KEYBOARD_STATE *key_state) {
    if (submissionState == SubmissionState::SENDING && asyncSubmissionResult.has_value()) {
        auto &submissionResult = asyncSubmissionResult.value();

        if (submissionResult.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                ApiResponse response = submissionResult.get();
                if (response.success) {
                    submissionState = SubmissionState::SUCCESS;
                    serverResponseMessage = response.message;
                } else {
                    submissionState = SubmissionState::FAILED;
                    serverResponseMessage = response.message;
                    std::cerr << "Falha na API: " << response.message << std::endl;
                }
            } catch (const std::exception &e) {
                submissionState = SubmissionState::FAILED;
                serverResponseMessage = "Erro crítico no envio.";
                std::cerr << "Exceção capturada da tarefa assíncrona: " << e.what() << std::endl;
            }
            this->pressKeyToRestartText = "Pressione [ENTER] para continuar";
            asyncSubmissionResult.reset();
        }
    } else if (submissionState != SubmissionState::SENDING) {
        if (al_key_down(key_state, ALLEGRO_KEY_ENTER) ||
            al_key_down(key_state, ALLEGRO_KEY_ESCAPE)) {
            this->gameManager->popScreen();
        }
    }
}
