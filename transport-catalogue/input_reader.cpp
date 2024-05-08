#include "input_reader.h"

namespace InputReaderParser {
    using Map_Str_Int = std::unordered_map<std::string_view, uint32_t>;

    //Парсит строку вида "9900m to Rasskazovka ... , 100m to Marushkino..." и возвращает u_map<name, distance>
    Map_Str_Int ParseDistances(std::string_view str, TransportCatalogue& catalogue) {
        Map_Str_Int dist;

        size_t comma_or_end = 0;
        while (comma_or_end != str.npos) {
            auto start = str.find_first_not_of(" ");
            auto m_pos = str.find_first_of("m");
            auto to_pos = str.find_first_not_of(" to", m_pos + 1);
            comma_or_end = str.find_first_of(",", to_pos);

            std::string name = std::string(str.substr(to_pos, comma_or_end - (to_pos)));
            if (catalogue.GetStop(name) == nullptr) { catalogue.AddStop(name, {}, {}); }
            auto stop = catalogue.GetStop(name);

            if (comma_or_end != str.npos) {
                dist[stop->stop_name] = std::stoi(std::string(str.substr(start, m_pos - start)));
                str = str.substr(comma_or_end + 1);
            }
            else {
                dist[stop->stop_name] = std::stoi(std::string(str.substr(start, m_pos - start)));
            }
        }
        return dist;
    }

    //Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
    geo::Coordinates ParseCoordinates(std::string_view str) {
        static const double nan = std::nan("");

        auto not_space = str.find_first_not_of(' ');
        auto comma = str.find(',');

        if (comma == str.npos) {
            return { nan, nan };
        }

        auto not_space2 = str.find_first_not_of(' ', comma + 1);

        double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
        double lng = std::stod(std::string(str.substr(not_space2)));

        return { lat, lng };
    }


    // 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"
    std::pair<geo::Coordinates, Map_Str_Int>  ParseCoordinatesAndDistances(std::string_view str, TransportCatalogue& catalogue) {

        auto comma = str.find(',');
        comma = str.find_first_of(',', comma + 1);

        geo::Coordinates coordinates = ParseCoordinates(str.substr(0, comma));
        Map_Str_Int stops_and_distances = ParseDistances(str.substr(comma + 1), catalogue);

        return { coordinates, stops_and_distances };
    }

    //Удаляет пробелы в начале и конце строки
    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    //Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
    std::vector<std::string_view> Split(std::string_view string, char delim) {
        std::vector<std::string_view> result;
        size_t pos = 0;

        while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
            auto delim_pos = string.find(delim, pos);
            if (delim_pos == string.npos) {
                delim_pos = string.size();
            }
            if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                result.push_back(substr);
            }
            pos = delim_pos + 1;
        }

        return result;
    }

    /**
    * Парсит маршрут.
    * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
    * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
    */
    std::vector<std::string_view> ParseRoute(std::string_view route) {
        if (route.find('>') != route.npos) {
            return Split(route, '>');
        }

        auto stops = Split(route, '-');
        std::vector<std::string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

        return results;
    }

    CommandDescription ParseCommandDescription(std::string_view line) {
        auto colon_pos = line.find(':');
        if (colon_pos == line.npos) {
            return {};
        }

        auto space_pos = line.find(' ');
        if (space_pos >= colon_pos) {
            return {};
        }

        auto not_space = line.find_first_not_of(' ', space_pos);
        if (not_space >= colon_pos) {
            return {};
        }

        return {std::string(line.substr(0, space_pos)),
                std::string(Trim(line.substr(not_space, colon_pos - not_space))),
                std::string(line.substr(colon_pos + 1)) };
    }
}

void FillCatalogue(std::istream& in, TransportCatalogue& catalogue) {
    int base_request_count;
    in >> base_request_count >> std::ws;

    InputReader reader;
    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        std::getline(in, line);
        reader.ParseLine(line);
    }
    reader.ApplyCommands(catalogue);
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = InputReaderParser::ParseCommandDescription(InputReaderParser::Trim(line));
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    for (auto command : commands_) {
        if (command.command == "Stop") {
            auto [coordinates, stops_and_distances] = InputReaderParser::ParseCoordinatesAndDistances(command.description, catalogue);
            catalogue.AddStop(command.id, coordinates, stops_and_distances);
        }
        else if (command.command == "Bus") {
            catalogue.AddBus(command.id, InputReaderParser::ParseRoute(command.description));
        }
    }
}