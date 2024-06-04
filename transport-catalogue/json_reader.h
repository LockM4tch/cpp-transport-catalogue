#pragma once
#include <unordered_map>
#include <string_view>
#include <iostream>
#include <sstream>
#include <memory>
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"


class  JsonReader
{
public:
    JsonReader(TransportCatalogue& catalogue, MapRenderer renderer);

    void ProcessRequest(const json::Document& doc, std::ostream& out);

private:
    void FillCatalogue(const json::Node& doc);
    void PrintStat(const json::Node& arr, std::ostream& out);

    void SetRenderSettings(const json::Node& node_map);

    json::Node GetStopInfo(const json::Node& node_map);
    json::Node GetBusInfo(const json::Node& node_map);
    json::Node GetMapInfo(const json::Node& node_map);

    std::unordered_map<std::string_view, size_t> CreateDistanceMap(const json::Dict& dict);
    std::vector<std::string_view> CreateRoute(json::Node& array);
    
    svg::Color MakeColor(const json::Node& node);
    
    TransportCatalogue& catalogue_;
    MapRenderer renderer_;

};//json_reader