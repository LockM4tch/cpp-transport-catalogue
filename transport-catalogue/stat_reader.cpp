#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output) {
    auto request_type = (request.substr(0, request.find_first_of(' ')));
    request = request.substr(request.find_first_of(' ') + 1, request.npos);

    if (request_type == "Bus") {
    output <<"Bus " << request << ": " << tansport_catalogue.CheckBus(request) << "\n";
    }
    else {
    output << "Stop " << request << ": " << tansport_catalogue.CheckStop(request) << "\n";
    }

}