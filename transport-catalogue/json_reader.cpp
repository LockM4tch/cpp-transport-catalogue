#include "json_reader.h"

namespace json_reader {
    using namespace std::literals;

    std::unordered_map<std::string_view, size_t> CreateDistanceMap(const json::Dict& dict, TransportCatalogue& catalogue) {
        std::unordered_map<std::string_view, size_t> out;
        for (auto d : dict)
        {
            if (catalogue.GetStop(d.first) == nullptr) { 
                catalogue.AddStop(d.first, {}, {});
            }
            out[catalogue.GetStop(d.first)->stop_name] = d.second.AsInt();
        }
        return out;
    }

    std::vector<std::string_view> CreateRoute(json::Node& array,TransportCatalogue& catalogue) {
        std::vector<std::string_view> out;

        auto arr = array.AsArray();
        for (auto stop : arr) {
            std::string stop_name = stop.AsString();
            if (catalogue.GetStop(stop_name) == nullptr) {
                catalogue.AddStop(stop_name, {}, {});
            }
            out.push_back(catalogue.GetStop(stop_name)->stop_name);
        }

       /* if (!is_roundtrip) {
            for (int i = static_cast<int>(arr.size()) - 2; i >= 0; --i) {
                out.push_back(out.at(i));
            }
        }*/
        return out;
    }

    void FillCatalogue(const json::Node& doc, TransportCatalogue& catalogue) {
        for (auto& v : doc.AsArray()) {
        json::Dict map = v.AsMap();

        if (map.at("type") == json::Node{ "Stop"s }) {
            auto distance = CreateDistanceMap(map["road_distances"].AsMap(), catalogue);
            catalogue.AddStop(map["name"].AsString(), { map["latitude"].AsDouble(), map["longitude"].AsDouble() }, distance);
        }
        else if (map.at("type") == json::Node{ "Bus"s }) {
            auto stops = CreateRoute(map["stops"], catalogue);
            catalogue.AddBus(map["name"].AsString(), stops, map["is_roundtrip"].AsBool());
        }
        }
    }

    json::Node GetStopInfo(const json::Node& node_map, TransportCatalogue& catalogue){
        const json::Dict& in = node_map.AsMap();
        json::Dict out;
        json::Array busses;

        auto stop = catalogue.GetStop(in.at("name").AsString());
        if (stop == nullptr) {
        out["request_id"] = in.at("id");
        out["error_message"] = "not found"s;
        }
        else {
        for (auto& bus : stop->buses) {
            busses.push_back(bus);
        }

        out["request_id"] = in.at("id");
        out["buses"] = busses;
        }
        return out;
    }

    json::Node GetBusInfo(const json::Node& node_map, TransportCatalogue& catalogue){
       const auto& bus = catalogue.GetBusStat(node_map.AsMap().at("name").AsString());
       json::Dict out;
       auto  id = node_map.AsMap().at("id");
       if (bus.has_value()) {
       out["request_id"] = id;
       out["curvature"] = bus.value().curvature;
       out["route_length"] = static_cast<int>(bus.value().route_length);
       out["stop_count"] = static_cast<int>(bus.value().stops_on_route);
       out["unique_stop_count"] = static_cast<int>(bus.value().unique_stops);
       }
       else {
           out["request_id"] = id;
           out["error_message"] = "not found"s;
       }

       return out;
    }

    json::Node GetMapInfo(const json::Node& node_map, TransportCatalogue& catalogue, MapRenderer& mr) {
        json::Dict out;
        out["request_id"] = node_map.AsMap().at("id");

        std::ostringstream outstream;
        mr.RenderMap(*catalogue.GetBuses(), outstream);
        std::string str = outstream.str();
        //outstream.clear();
        //json::Indent ind;
        //json::PrintNode(str, outstream, ind);
        out["map"] = outstream.str();
        return out;
    }

    void PrintStat(const json::Node& arr, TransportCatalogue& catalogue, MapRenderer& mr, std::ostream& out){
        if (arr.AsArray().empty()) { return; }

        json::Array vctr;
        for (auto& elem : arr.AsArray())
        {
            std::string type = elem.AsMap().at("type").AsString();
            if (type == "Stop") {
                vctr.push_back(GetStopInfo(elem, catalogue));
            }
            else if (type == "Bus") {
                vctr.push_back(GetBusInfo(elem, catalogue));
            }
            else if (type == "Map") {
                vctr.push_back(GetMapInfo(elem, catalogue, mr));

            }
        }
        json::Print(json::Document{vctr}, out);
    }

    svg::Color MakeColor(const json::Node& node) {
        
        if (node.IsArray()) {
            uint8_t r = static_cast<uint8_t>(node.AsArray().at(0).AsInt());
            uint8_t g = static_cast<uint8_t>(node.AsArray().at(1).AsInt());
            uint8_t b = static_cast<uint8_t>(node.AsArray().at(2).AsInt());
            if (node.AsArray().size() == 3) {
                return svg::Rgb({r,g,b });
            }
            else
            {
                double a = node.AsArray().at(3).AsDouble();
                return svg::Rgba(r,g,b,a);
            }
        }
        else {
            return node.AsString();
        }

    }

    void SetRenderSettings(const json::Node& node_map, MapRenderer& renderer) {
        const json::Dict& map = node_map.AsMap();
        
        const double height = map.at("height").AsDouble();
        const double width = map.at("width").AsDouble();
        const double padding = map.at("padding").AsDouble();
        const double line_width = map.at("line_width").AsDouble();
        const double stop_radius = map.at("stop_radius").AsDouble();
        const int bus_label_font_size = map.at("bus_label_font_size").AsInt();
        const int stop_label_font_size = map.at("stop_label_font_size").AsInt();
        const svg::Point bus_label_offset = {
            map.at("bus_label_offset").AsArray()[0].AsDouble(),
            map.at("bus_label_offset").AsArray()[1].AsDouble()
        };
        const svg::Point stop_label_offset = {
            map.at("stop_label_offset").AsArray()[0].AsDouble(),
            map.at("stop_label_offset").AsArray()[1].AsDouble()
        };

        renderer.SetHeight(height)
            .SetWidth(width)
            .SetPadding(padding)
            .SetLineWidth(line_width)
            .SetStopRadius(stop_radius)
            .SetBusLabelFontSize(bus_label_font_size)
            .SetBusLabelOffset(bus_label_offset)
            .SetStopLabelFontSize(stop_label_font_size)
            .SetStopLabelOffset(stop_label_offset);

        svg::Color underlayer_color = MakeColor(map.at("underlayer_color"));
        renderer.SetUnderlayerColor(underlayer_color).SetUnderlayerWidth(map.at("underlayer_width").AsDouble());
        
        std::vector<svg::Color> color_palette;
        for (auto v : map.at("color_palette").AsArray())
        {
            color_palette.push_back(MakeColor(v));
        }
        renderer.SetColorPalette(color_palette);
    }

    void ProcessRequest(const json::Document& doc, TransportCatalogue& catalogue, MapRenderer& mr, std::ostream& out){
        auto& map = doc.GetRoot().AsMap();
        
        if (map.count("base_requests") != 0 ) {
            FillCatalogue(map.at("base_requests").AsArray(), catalogue);
        }
        if (map.count("render_settings") != 0) {
            SetRenderSettings(map.at("render_settings").AsMap(), mr);
        }
        if (map.count("stat_requests") != 0) {
            PrintStat(map.at("stat_requests").AsArray(), catalogue, mr, out);
        }
    }

}// namespace json_reader