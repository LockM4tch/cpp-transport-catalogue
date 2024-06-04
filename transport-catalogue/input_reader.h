#pragma once
#include "geo.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

namespace InputReaderParser {

struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};
}

void FillCatalogue(std::istream& in, TransportCatalogue& catalogue);

class InputReader {
public:
    void ParseLine(std::string_view line);

    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    std::vector<InputReaderParser::CommandDescription> commands_;
};