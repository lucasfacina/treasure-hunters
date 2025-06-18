#include "serial_port.h"

// ======================================================================
// IMPLEMENTAÇÃO PARA WINDOWS
// ======================================================================
#ifdef _WIN32
#include <windows.h>

SerialPort::SerialPort(const std::string& port, unsigned int baudRate) {
    std::string portName = "\\\\.\\" + port;
    this->hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (this->hSerial == INVALID_HANDLE_VALUE) {
        throw SerialException("Erro ao abrir a porta serial: " + port);
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(this->hSerial, &dcbSerialParams)) {
        CloseHandle(this->hSerial);
        throw SerialException("Erro ao obter estado da porta serial.");
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(this->hSerial, &dcbSerialParams)) {
        CloseHandle(this->hSerial);
        throw SerialException("Erro ao configurar a porta serial.");
    }

    // Configura timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    if(!SetCommTimeouts(this->hSerial, &timeouts)) {
        CloseHandle(this->hSerial);
        throw SerialException("Erro ao configurar timeouts.");
    }

    this->connected = true;
    PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
    Sleep(2000); // Delay para o Arduino reiniciar
}

SerialPort::~SerialPort() {
    if (this->connected) {
        CloseHandle(this->hSerial);
    }
}

std::string SerialPort::readLine() {
    if (!this->connected) return "";

    std::string line;
    char buffer[2] = {0}; // Lê um caractere por vez
    DWORD bytesRead = 0;

    while (ReadFile(this->hSerial, buffer, 1, &bytesRead, NULL) && bytesRead > 0) {
        if (buffer[0] == '\n') {
            break;
        }
        line += buffer[0];
    }

    // Remove o '\r' que o Arduino pode enviar com println
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    return line;
}

#else
// ======================================================================
// IMPLEMENTAÇÃO PARA LINUX & MACOS (POSIX)
// ======================================================================
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

SerialPort::SerialPort(const std::string &port, unsigned int baudRate) {
    this->fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (this->fd == -1) {
        throw SerialException("Erro ao abrir a porta serial: " + port);
    }

    termios options{};
    tcgetattr(this->fd, &options);

    // Configura baud rate
    cfsetispeed(&options, B9600); // Sempre usar 9600, conforme código do Arduino
    cfsetospeed(&options, B9600);

    // Configura modo canônico, 8 bits de dados, sem paridade, 1 stop bit
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= CREAD | CLOCAL; // Habilita leitura e ignora sinais de controle do modem
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Modo não-canônico
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Desabilita controle de fluxo por software

    options.c_cc[VMIN] = 0; // Leitura não bloqueante
    options.c_cc[VTIME] = 1; // Timeout de 0.1 segundos

    tcsetattr(this->fd, TCSANOW, &options);

    this->connected = true;
    usleep(2000 * 1000); // Delay de 2s para o Arduino reiniciar
}

SerialPort::~SerialPort() {
    if (this->connected) {
        close(this->fd);
    }
}

std::string SerialPort::readLine() const {
    if (!this->connected) return "";

    std::string line;
    char buffer[2] = {0};

    while (read(this->fd, buffer, 1) > 0) {
        if (buffer[0] == '\n') {
            break;
        }
        line += buffer[0];
    }

    // Remove o '\r' que o Arduino pode enviar com println
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    return line;
}
#endif

// Função comum a ambas as implementações
bool SerialPort::isConnected() const {
    return this->connected;
}
