#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <iostream>
#include <string>

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "screens/game_over_info.h"

using json = nlohmann::json;

template <typename T = void>
struct ApiResponse {
    bool success{};
    std::string message;
    T data{};
};

template <>
struct ApiResponse<void> {
    bool success{};
    std::string message;
};

enum class SubmissionState {
    SENDING,
    SUCCESS,
    FAILED
};

class ApiClient {
    std::string api_key;
    std::string api_host;
    std::string api_path;

    void loadEnvVars() {
        const char *key = std::getenv("GAME_CLIENT_API_KEY");
        const char *host = std::getenv("API_HOST");
        const char *path = std::getenv("API_PATH");

        if (key) {
            this->api_key = key;
        } else {
            std::cerr << "AVISO: Variável de ambiente GAME_CLIENT_API_KEY não definida." << std::endl;
        }

        if (host) {
            this->api_host = host;
        } else {
            std::cerr << "AVISO: Variável de ambiente API_HOST não definida." << std::endl;
            this->api_host = "http://localhost:3000";
        }

        if (path) {
            this->api_path = path;
        } else {
            std::cerr << "AVISO: Variável de ambiente API_PATH não definida." << std::endl;
            this->api_path = "/api/trpc/gameMatch.end";
        }
    }

public:
    ApiClient() { this->loadEnvVars(); }

    ApiResponse<> submitGameResults(const GameOverInfo &results) {
        if (api_key.empty()) {
            std::cerr << "Erro: API Key não configurada. Não foi possível enviar os resultados." << std::endl;
            return {false, "Erro: API Key não configurada no cliente."};
        }

        json payload;
        payload["blueScore"] = results.blueScore;
        payload["pinkScore"] = results.pinkScore;
        payload["durationSeconds"] = results.matchDurationInSeconds;

        json collectedItems = json::array();
        for (const auto &[playerType, houseSlots]: results.houseSlotByPlayerType) {
            for (const auto &houseSlot: houseSlots) {
                if (houseSlot->getStoredTresureItem() == nullptr) continue;

                json item_obj;
                item_obj["depositedBy"] = playerTypeToString(houseSlot->getDepositedBy().value());
                item_obj["depositedAt"] = playerTypeToString(houseSlot->getType());
                item_obj["itemId"] = houseSlot->getStoredTresureItem()->getItemId();
                collectedItems.push_back(item_obj);
            }
        }
        payload["collectedItems"] = collectedItems;

        json req_envelope;
        req_envelope["json"] = payload;
        std::string req_body = req_envelope.dump();

        httplib::Headers headers = {
            {"Content-Type", "application/json"},
            {"x-api-key", this->api_key}
        };

        std::cout << "Enviando resultados para: " << this->api_host << this->api_path << std::endl;

        httplib::Result res;
        if (this->api_host.rfind("https://", 0) == 0) {
            /*
             * O "https://" na variável de ambiente é
             * importante para sabermos qual protocolo utilizar,
             * porém o SSLClient não espera que tenha isso
             * no início de uma conexão, por isso é importante
             * removermos da ap_host.
             */
            std::string host = this->api_host.substr(8);

            httplib::SSLClient client(host);
            client.enable_server_certificate_verification(false);
            res = client.Post(
                this->api_path,
                headers,
                req_body,
                "application/json"
            );
        } else {
            httplib::Client client(this->api_host);
            res = client.Post(
                this->api_path,
                headers,
                req_body,
                "application/json"
            );
        }

        if (res && res->status == 200) {
            std::cout << "Resultados enviados com sucesso!" << std::endl;
            try {
                json json_response = json::parse(res->body);

                auto data = json_response["result"]["data"]["json"];

                ApiResponse<> response;
                response.success = data.value("success", false);
                response.message = data.value("message", "Resposta sem mensagem.");

                return response;
            } catch (const json::parse_error &e) {
                std::cerr << "Erro ao parsear a resposta JSON do servidor: " << e.what() << std::endl;
                return {false, "Erro ao ler a resposta do servidor."};
            }
        }

        std::cerr << "Falha ao enviar resultados." << std::endl;
        if (res) {
            std::cerr << "Status: " << res->status << std::endl;
            std::cerr << "Body: " << res->body << std::endl;
            std::string error_message = "Falha no envio. Status: " + std::to_string(res->status);
            return {false, error_message};
        }

        auto err = res.error();
        std::cerr << "Erro de HTTP: " << httplib::to_string(err) << std::endl;
        std::string error_message = "Erro de conexão: " + httplib::to_string(err);
        return {false, error_message};
    }
};

#endif //API_CLIENT_H
