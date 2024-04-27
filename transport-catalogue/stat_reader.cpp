#include "stat_reader.h"

void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output) {

    auto bus_stat = transport_catalogue.GetBusStat(request);
    if (bus_stat.has_value()) {
    auto [stops_on_route, unique_stops, route_length] = bus_stat.value();
    output << "Bus " << request << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length" << "\n";
    }
    else { 
        output << "Bus " << request << ": not found\n"; 
    }
}

void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output) {
    output << "Stop " << request << ": ";
    ;
    if (transport_catalogue.GetStop(request) == nullptr) {
        output << "not found\n"; 
        return; 
    }
    
    auto buses = transport_catalogue.GetStop(request)->buses;
    if (buses.empty()) {
        output << "no buses\n";
    }
    else{
        output << "buses";
        for (auto bus : buses) {
            output << " " << bus;
        }
        output << "\n";
    }
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output) {
    auto request_type = (request.substr(0, request.find_first_of(' ')));
    request = request.substr(request.find_first_of(' ') + 1, request.npos);

    if (request_type == "Bus") {
        PrintBusInfo(transport_catalogue, request, output);
    }
    else {
        PrintStopInfo(transport_catalogue, request, output);
    }
}

void ExecuteStatRequests(std::istream& in ,std::ostream& out, TransportCatalogue& transport_catalogue) {
    int stat_request_count;
    in >> stat_request_count >> std::ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        std::getline(std::cin, line);
        ParseAndPrintStat(transport_catalogue, line, out);
    }

}