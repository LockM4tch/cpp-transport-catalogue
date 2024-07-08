#include "input_reader.h"

namespace InputReaderParser {
    using DistanceMap = std::unordered_map<std::string, int>;
    //                    01234567890    m|to|name,
    //Парсит строку вида " 9900m to Rasskazovka ... , 100m to Marushkino..." и возвращает u_map<name, distance>
    DistanceMap ParseDistances(std::string_view str) {
        DistanceMap dist;

        size_t comma_or_end = 0;
        while (comma_or_end != str.npos) {
            auto start = str.find_first_not_of(" ");
            auto m_pos = str.find_first_of("m");
            auto not_space = str.find_first_not_of(" ", m_pos+1);
                not_space = str.find_first_of(" ", not_space+1);
                not_space = str.find_first_not_of(" ", not_space + 1);

            comma_or_end = str.find_first_of(",", not_space);

            std::string name = std::string(str.substr(not_space, comma_or_end - (not_space)));

            if (comma_or_end != str.npos) {
                dist[name] = std::stoi(std::string(str.substr(start, m_pos - start)));
                str = str.substr(comma_or_end + 1);
            }
            else {
                dist[name] = std::stoi(std::string(str.substr(start, m_pos - start)));
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

    struct StopWithDistance
    {
        Stop stop;
        DistanceMap distance_map;
    };

    // 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino"
    StopWithDistance ParseStopAndDistances(std::string_view name, std::string_view str) {
        std::string name_ = std::string(name);

        auto comma = str.find(',');
        comma = str.find_first_of(',', comma + 1);

        geo::Coordinates coordinates = ParseCoordinates(str.substr(0, comma));
        Stop stop = {0, std::string(name), coordinates };

        DistanceMap stops_and_distances;
        if (comma != str.npos) {
        stops_and_distances = ParseDistances(str.substr(comma + 1));
        }
            return {stop, stops_and_distances};
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

        return { std::string(line.substr(0, space_pos)),
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
            auto [stop, distance] = InputReaderParser::ParseStopAndDistances(command.id, command.description);
            catalogue.AddStop(stop);


            for (auto [other_stop_name, dist] : distance) {
                if (catalogue.GetStop(other_stop_name) == nullptr) {
                    catalogue.AddStop(other_stop_name, {}); 
                }
                catalogue.SetDistance(catalogue.GetStop(stop.stop_name), catalogue.GetStop(other_stop_name), dist);
            }
        }
        else if (command.command == "Bus") {
            bool isRoundtrip = true;
            catalogue.AddBus(command.id, InputReaderParser::ParseRoute(command.description), isRoundtrip);
        }
    }
}