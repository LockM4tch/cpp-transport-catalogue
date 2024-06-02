#pragma once
#include <unordered_map>
#include <string_view>
#include <iostream>
#include <sstream>
#include <memory>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"



namespace json_reader {
    std::unordered_map<std::string_view, size_t> CreateDistanceMap(const json::Dict& dict, TransportCatalogue& catalogue);

    std::vector<std::string_view> CreateRoute(json::Node& array, TransportCatalogue& catalogue);

    void FillCatalogue(const json::Node& doc, TransportCatalogue& catalogue);

    void ProcessRequest(const json::Document& doc, TransportCatalogue& catalogue, MapRenderer& mr, std::ostream& out);

    json::Node GetStopInfo(const json::Node& node_map, TransportCatalogue& catalogue);

    json::Node GetBusInfo(const json::Node& node_map, TransportCatalogue& catalogue);

    void PrintStat(const json::Node& arr, TransportCatalogue& catalogue, MapRenderer& mr, std::ostream& out);

}//json_reader