#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <iostream>
#include <string>
#include <fstream>

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "screens/game_over_info.h"

using json = nlohmann::json;

template<typename T = void>
struct ApiResponse {
    bool success{};
    std::string message;
    T data{};
};

template<>
struct ApiResponse<void> {
    bool success{};
    std::string message;
};

struct IsRankedMatchResponse {
    bool isRanked{};
    std::string bluePlayerName;
    std::string pinkPlayerName;
};

enum class SubmissionState {
    SENDING,
    SUCCESS,
    FAILED
};

class ApiClient {
    std::string api_key;
    std::string api_host;
    std::string api_base_path;
    httplib::Headers headers;

    void loadEnvVars() {
        std::string config_path = "config.json";
        std::ifstream config_file(config_path);

        if (!config_file.is_open()) {
            std::cerr << "ERRO FATAL: Não foi possível abrir '" << config_path << "'." << std::endl;
            std::cerr << "Verifique se o arquivo existe ao lado do .exe (dentro da pasta assets)." << std::endl;

            this->api_host = "http://localhost:3000";
            this->api_base_path = "/api/trpc/";
            this->api_key = "";
            std::cerr << "AVISO: API Key não carregada." << std::endl;
            return;
        }

        try {
            json config = json::parse(config_file);

            this->api_key = config.value("apiKey", "");
            this->api_host = config.value("apiHost", "http://localhost:3000");
            this->api_base_path = config.value("apiBasePath", "/api/trpc/");

            if (this->api_key.empty()) {
                std::cerr << "AVISO: 'apiKey' está vazia ou não foi encontrada em '" << config_path << "'." << std::endl;
            }

            std::cout << "Configuracao da API carregada de '" << config_path << "'." << std::endl;
            std::cout << "Host da API: " << this->api_host << std::endl;

        } catch (const json::parse_error& e) {
            std::cerr << "ERRO FATAL: Erro ao ler '" << config_path << "': " << e.what() << std::endl;

            this->api_host = "http://localhost:3000";
            this->api_base_path = "/api/trpc/";
            this->api_key = "";
            std::cerr << "AVISO: API Key não carregada." << std::endl;
        }
    }

    nlohmann::json_abi_v3_11_3::basic_json<> _parseResponse(const std::string &body) {
        try {
            json json_response = json::parse(body);
            return json_response["result"]["data"]["json"];
        } catch (const json::parse_error &e) {
            std::cerr << "Erro ao parsear a resposta JSON do servidor: " << e.what() << std::endl;
            return nullptr;
        }
    }

    nlohmann::json_abi_v3_11_3::basic_json<> _post(const std::string &endpoint, const json &payload) {
        if (api_key.empty()) {
            std::cerr << "Erro: API Key não configurada. Não foi possível enviar os resultados." << std::endl;
            return nullptr;
        }

        json req_envelope;
        req_envelope["json"] = payload;
        std::string req_body = req_envelope.dump();

        auto full_path = this->api_base_path + endpoint;
        std::cout << "[POST] em " << this->api_host << full_path << std::endl;

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
                full_path,
                headers,
                req_body,
                "application/json"
            );
        } else {
            httplib::Client client(this->api_host);
            res = client.Post(
                full_path,
                headers,
                req_body,
                "application/json"
            );
        }

        if (res && res->status == 200) {
            std::cout << "Requisição enviada com sucesso!" << std::endl;
            return this->_parseResponse(res->body);
        }

        return nullptr;
    }

    nlohmann::json_abi_v3_11_3::basic_json<> _get(const std::string &endpoint) {
        if (api_key.empty()) {
            std::cerr << "Erro: API Key não configurada. Não foi possível enviar os resultados." << std::endl;
            return nullptr;
        }

        auto full_path = this->api_base_path + endpoint;
        std::cout << "[GET] em " << this->api_host << full_path << std::endl;

        httplib::Result res;
        if (this->api_host.rfind("https://", 0) == 0) {
            std::string host = this->api_host.substr(8);

            httplib::SSLClient client(host);
            client.enable_server_certificate_verification(false);
            res = client.Get(full_path, headers);
        } else {
            httplib::Client client(this->api_host);
            res = client.Get(full_path, headers);
        }

        if (res && res->status == 200) {
            std::cout << "Requisição enviada com sucesso!" << std::endl;
            return this->_parseResponse(res->body);
        }

        return nullptr;
    }

public:
    ApiClient() {
        this->loadEnvVars();

        this->headers = {
            {"Content-Type", "application/json"},
            {"x-api-key", this->api_key}
        };
    }

    ApiResponse<> submitGameResults(const GameOverInfo &results) {
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

        auto response_data = this->_post("gameMatch.end", payload);
        if (response_data.is_null())
            return {false, "Falha ao enviar resultados. Sem resposta do servidor."};

        return {true, response_data.value("message", "Resultados enviados com sucesso.")};
    }

    ApiResponse<IsRankedMatchResponse> isRankedMatch() {
        const auto data = this->_get("gameMatch.isRankedMatch");
        if (data.is_null())
            return {
                .success = false,
                .message = "Falha ao verificar se a partida é ranqueada. Sem resposta do servidor.",
                .data = {
                    .isRanked = false,
                    .bluePlayerName = "Azul",
                    .pinkPlayerName = "Rosa"
                }
            };

        return {
            .success = true,
            .message = data.value("message", "Partida ranqueada verificada com sucesso."),
            .data = {
                .isRanked = data.value("isRanked", false),
                .bluePlayerName = data.value("bluePlayerName", "Azul"),
                .pinkPlayerName = data.value("pinkPlayerName", "Rosa")
            }
        };
    }
};

#endif //API_CLIENT_H
