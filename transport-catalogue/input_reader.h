#pragma once
#include "geo.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace InputReaderParser {

struct CommandDescription {
    // ����������, ������ �� ������� (���� command ��������)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // �������� �������
    std::string id;           // id �������� ��� ���������
    std::string description;  // ��������� �������
};
}

void FillCatalogue(std::istream& in, TransportCatalogue& catalogue);

class InputReader {
public:
    //������ ������ � ��������� CommandDescription � ��������� ��������� � commands_
    void ParseLine(std::string_view line);

    // ��������� ������� ������������ ����������, ��������� ������� �� commands_
    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    std::vector<InputReaderParser::CommandDescription> commands_;
};