#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
#include <vector>
#include <stdexcept>

// Exceção customizada para erros de serial
class SerialException final : public std::runtime_error {
public:
    explicit SerialException(const std::string &message) : std::runtime_error(message) {}
};

class SerialPort {
public:
    // Construtor: Abre a porta serial
    SerialPort(const std::string &port, unsigned int baudRate);

    // Destrutor: Fecha a porta serial
    ~SerialPort();

    // Lê uma linha da porta serial (terminada em '\n')
    [[nodiscard]] std::string readLine() const;

    // Verifica se a porta está conectada
    [[nodiscard]] bool isConnected() const;

private:
    static std::vector<std::string> listAvailablePorts();

    static std::string selectPortFromList();

    // Detalhes de implementação escondidos do usuário da classe
    // Variáveis específicas de cada SO serão declaradas aqui
    bool connected = false;

#ifdef _WIN32
    void *hSerial; // Usamos void* para não precisar incluir <windows.h> no header
#else
    int fd; // File descriptor para Linux/macOS
#endif
};

#endif // SERIAL_PORT_H
