#pragma once
#include <unordered_map>
#include <string_view>
#include <iostream>
#include <sstream>
#include <memory>

#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "transport_catalogue.h"
#include "json_builder.h"


class  JsonReader
{
public:
    JsonReader(TransportCatalogue& catalogue, MapRenderer& renderer, TransportRouter& router);

    void ProcessRequest(const json::Document& doc, std::ostream& out);


private:
    void SetRouteSettings(const json::Node& node_map);

    void FillCatalogue(const json::Node& doc);
    void PrintStat(const json::Node& arr, std::ostream& out);

    void SetRenderSettings(const json::Node& node_map);

    json::Node::Value GetStopInfo(const json::Node& node_map);
    json::Node::Value GetBusInfo(const json::Node& node_map);
    json::Node::Value GetMapInfo(const json::Node& node_map);
    json::Node::Value GetRouteInfo(const json::Node& node_map);

    std::unordered_map<std::string_view, size_t> CreateDistanceMap(const json::Dict& dict);
    std::vector<std::string_view> CreateRoute(json::Node& array);
    
    svg::Color MakeColor(const json::Node& node);
    
    TransportCatalogue& catalogue_;
    MapRenderer& renderer_;
    TransportRouter& router_;
};//json_reader