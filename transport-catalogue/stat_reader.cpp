#include "stat_reader.h"

void PrintBusInfo(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output) {
   auto bus_it = tansport_catalogue.Get_Bus_By_Name(request);

   if (bus_it == nullptr) {
       output << "Bus " << request << ": " << "not found\n";
   }
   else {
       int stops_on_route = bus_it->second.stops.size(); ;
       float route_length = 0;
       std::unordered_set<std::string> unique_stops;

       for (int i = 1; i < stops_on_route; ++i) {
           auto [stop_name, coordinates_first] = *tansport_catalogue.Get_Stop_By_Name(bus_it->second.stops.at(i - 1));
           auto [stop_name2, coordinates_second] = *tansport_catalogue.Get_Stop_By_Name(bus_it->second.stops.at(i));

           unique_stops.insert(stop_name);
           route_length += geo::ComputeDistance(coordinates_first.coordinates, coordinates_second.coordinates);
       }
    output << "Bus " << request << ": " << stops_on_route << " stops on route, " << unique_stops.size() << " unique stops, " << route_length << " route length" << "\n";
   }
}

void PrintStopInfo(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output) {
    output << "Stop " << request << ": ";
    ;
    if (tansport_catalogue.Get_Stop_By_Name(request) == nullptr) {
        output << "not found\n"; 
        return; 
    }
    
    auto buses = tansport_catalogue.Get_Stop_By_Name(request)->second.buses;
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

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output) {
    auto request_type = (request.substr(0, request.find_first_of(' ')));
    request = request.substr(request.find_first_of(' ') + 1, request.npos);

    if (request_type == "Bus") {
        PrintBusInfo(tansport_catalogue, request, output);
    }
    else {
        PrintStopInfo(tansport_catalogue, request, output);
    }
}

void Request_Stat(std::istream& in ,std::ostream& out, TransportCatalogue& tansport_catalogue) {
    int stat_request_count;
    in >> stat_request_count >> std::ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        std::getline(std::cin, line);
        ParseAndPrintStat(tansport_catalogue, line, out);
    }

}