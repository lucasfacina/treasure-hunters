#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <iostream>
#include <string>

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "screens/game_over_info.h"

using json = nlohmann::json;

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

    bool submitGameResults(const GameOverInfo &results) {
        if (api_key.empty()) {
            std::cerr << "Erro: API Key não configurada. Não foi possível enviar os resultados." << std::endl;
            return false;
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

            httplib::SSLClient client(host.c_str());
            client.enable_server_certificate_verification(false);
            res = client.Post(
                this->api_path.c_str(),
                headers,
                req_body,
                "application/json"
            );
        } else {
            httplib::Client client(this->api_host.c_str());
            res = client.Post(
                this->api_path.c_str(),
                headers,
                req_body,
                "application/json"
            );
        }

        if (res && res->status == 200) {
            std::cout << "Resultados enviados com sucesso!" << std::endl;
            std::cout << "Resposta do servidor: " << res->body << std::endl;
            return true;
        }

        std::cerr << "Falha ao enviar resultados." << std::endl;
        if (res) {
            std::cerr << "Status: " << res->status << std::endl;
            std::cerr << "Body: " << res->body << std::endl;
        } else {
            auto err = res.error();
            std::cerr << "Erro de HTTP: " << httplib::to_string(err) << std::endl;
        }
        return false;
    }
};

#endif //API_CLIENT_H
