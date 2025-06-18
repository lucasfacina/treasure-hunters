#include "serial_port.h"
#include <iostream>
#include <limits>

// ======================================================================
// IMPLEMENTAÇÃO PARA WINDOWS
// ======================================================================
#ifdef _WIN32
#include <windows.h>

std::vector<std::string> SerialPort::listAvailablePorts() {
    std::vector<std::string> ports;

    // Tenta acessar portas COM de 1 a 20
    for (int i = 1; i <= 20; ++i) {
        std::string portName = "COM" + std::to_string(i);
        std::string fullPortName = "\\\\.\\" + portName;

        HANDLE hSerial = CreateFile(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hSerial != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(hSerial);
        }
    }

    return ports;
}

std::string SerialPort::selectPortFromList() {
    std::vector<std::string> availablePorts = listAvailablePorts();

    if (availablePorts.empty()) {
        throw SerialException("Nenhuma porta serial foi encontrada!");
    }

    std::cout << "\nPortas encontradas:" << std::endl;
    std::cout << "[0] Sair - Não utilizar porta serial" << std::endl;

    for (size_t i = 0; i < availablePorts.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << availablePorts[i] << std::endl;
    }

    std::cout << "\nQual porta deseja utilizar?" << std::endl;
    std::cout << "> ";

    int choice;
    while (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(availablePorts.size())) {
        std::cout << "Escolha inválida! Digite um número entre 0 e " << availablePorts.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer

    if (choice == 0) {
        std::cout << "Nenhuma porta serial será utilizada." << std::endl;
        return "";
    }

    return availablePorts[choice - 1];
}

SerialPort::SerialPort(const std::string& port, unsigned int baudRate) {
    std::string selectedPort = port;

    // Se o port estiver vazio ou for nullptr, lista as portas disponíveis
    if (port.empty()) {
        selectedPort = selectPortFromList();
    }

    if (selectedPort.empty()) return;

    std::string portName = "\\\\.\\" + selectedPort;
    this->hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (this->hSerial == INVALID_HANDLE_VALUE) {
        // Se falhar e o port original não estava vazio, tenta listar as portas
        if (!port.empty()) {
            std::cout << "Erro ao conectar com a porta: " << port << std::endl;
            selectedPort = selectPortFromList();
            portName = "\\\\.\\" + selectedPort;
            this->hSerial = CreateFile(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (this->hSerial == INVALID_HANDLE_VALUE) {
                throw SerialException("Erro ao abrir a porta serial: " + selectedPort);
            }
        } else {
            throw SerialException("Erro ao abrir a porta serial: " + selectedPort);
        }
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
#include <dirent.h>
#include <sys/stat.h>

std::vector<std::string> SerialPort::listAvailablePorts() {
    std::vector<std::string> ports;

    DIR *dir = opendir("/dev");
    if (dir == nullptr) {
        return ports;
    }

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        // Filtrar portas seriais típicas no Linux/macOS
        if (name.find("ttyUSB") == 0 ||
            name.find("ttyACM") == 0 ||
            name.find("tty.usbmodem") == 0 ||
            name.find("tty.usbserial") == 0 ||
            name.find("cu.usbmodem") == 0 ||
            name.find("cu.usbserial") == 0) {
            std::string fullPath = "/dev/" + name;

            // Verifica se é um dispositivo de caractere válido
            struct stat st{};
            if (stat(fullPath.c_str(), &st) == 0 && S_ISCHR(st.st_mode)) {
                ports.push_back(fullPath);
            }
        }
    }

    closedir(dir);
    return ports;
}

std::string SerialPort::selectPortFromList() {
    std::vector<std::string> availablePorts = listAvailablePorts();

    if (availablePorts.empty()) {
        throw SerialException("Nenhuma porta serial foi encontrada!");
    }

    std::cout << "\nPortas encontradas:" << std::endl;
    std::cout << "[0] Sair - Não utilizar porta serial" << std::endl;

    for (size_t i = 0; i < availablePorts.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << availablePorts[i] << std::endl;
    }

    std::cout << "\nQual porta deseja utilizar?" << std::endl;
    std::cout << "> ";

    int choice;
    while (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(availablePorts.size())) {
        std::cout << "Escolha inválida! Digite um número entre 0 e " << availablePorts.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer

    if (choice == 0) {
        std::cout << "Nenhuma porta serial será utilizada." << std::endl;
        return "";
    }

    return availablePorts[choice - 1];
}

SerialPort::SerialPort(const std::string &port, unsigned int baudRate) {
    std::string selectedPort = port;

    // Se o port estiver vazio, lista as portas disponíveis
    if (port.empty()) {
        selectedPort = selectPortFromList();
    }

    if (selectedPort.empty()) return;

    this->fd = open(selectedPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (this->fd == -1) {
        // Se falhar e o port original não estava vazio, tenta listar as portas
        if (!port.empty()) {
            std::cout << "Erro ao conectar com a porta: " << port << std::endl;
            selectedPort = selectPortFromList();
            this->fd = open(selectedPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

            if (this->fd == -1) {
                throw SerialException("Erro ao abrir a porta serial: " + selectedPort);
            }
        } else {
            throw SerialException("Erro ao abrir a porta serial: " + selectedPort);
        }
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
