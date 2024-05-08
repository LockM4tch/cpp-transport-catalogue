#include "input_reader.h"

namespace InputReaderParser {
    //Ïàðñèò ñòðîêó âèäà "10.123,  -30.1837" è âîçâðàùàåò ïàðó êîîðäèíàò (øèðîòà, äîëãîòà)
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

    //Óäàëÿåò ïðîáåëû â íà÷àëå è êîíöå ñòðîêè
    std::string_view Trim(std::string_view string) {
        const auto start = string.find_first_not_of(' ');
        if (start == string.npos) {
            return {};
        }
        return string.substr(start, string.find_last_not_of(' ') + 1 - start);
    }

    //Ðàçáèâàåò ñòðîêó string íà n ñòðîê, ñ ïîìîùüþ óêàçàííîãî ñèìâîëà-ðàçäåëèòåëÿ delim
    std::vector<std::string_view> Split(std::string_view string, char delim) {
        std::vector<std::string_view> result;

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
    * Ïàðñèò ìàðøðóò.
    * Äëÿ êîëüöåâîãî ìàðøðóòà (A>B>C>A) âîçâðàùàåò ìàññèâ íàçâàíèé îñòàíîâîê [A,B,C,A]
    * Äëÿ íåêîëüöåâîãî ìàðøðóòà (A-B-C-D) âîçâðàùàåò ìàññèâ íàçâàíèé îñòàíîâîê [A,B,C,D,C,B,A]
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
                catalogue.AddStop(command.id, InputReaderParser::ParseCoordinates(command.description));
            }
            else if (command.command == "Bus") {
                catalogue.AddBus(command.id, InputReaderParser::ParseRoute(command.description));
            }
        }
    }
