#ifndef FORMAT_TIME_H
#define FORMAT_TIME_H

#include <format>
#include <string>

inline std::string formatTime(long long seconds) {
    if (seconds <= 0) return "0 segundos";
    if (seconds == 1) return "1 segundo";
    if (seconds < 60) return std::format("{} segundos", seconds);

    const auto minutes = seconds / 60;
    const auto remaining_seconds = seconds % 60;

    std::string result;
    if (minutes == 1) result = "1 minuto";
    else result = std::format("{} minutos", minutes);

    if (remaining_seconds > 0) result += " e " + formatTime(remaining_seconds);

    return result;
}

#endif //FORMAT_TIME_H
