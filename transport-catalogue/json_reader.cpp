#include "json_reader.h"

using namespace std::literals;

JsonReader::JsonReader(TransportCatalogue& catalogue, MapRenderer& renderer, TransportRouter& router) : catalogue_(catalogue), renderer_(renderer), router_(router) {
}

std::unordered_map<std::string_view, size_t> JsonReader::CreateDistanceMap(const json::Dict& dict) {
    std::unordered_map<std::string_view, size_t> out;
    for (auto distance_pair : dict)
    {
        if (catalogue_.GetStop(distance_pair.first) == nullptr) {
            catalogue_.AddStop(distance_pair.first, {});
        }
        out[catalogue_.GetStop(distance_pair.first)->stop_name] = distance_pair.second.AsInt();
    }
    return out;
}

std::vector<std::string_view> JsonReader::CreateRoute(json::Node& array) {
    std::vector<std::string_view> out;

    auto arr = array.AsArray();
    for (auto stop : arr) {
        std::string stop_name = stop.AsString();
        if (catalogue_.GetStop(stop_name) == nullptr) {
            catalogue_.AddStop(stop_name, {});
        }
        out.push_back(catalogue_.GetStop(stop_name)->stop_name);
    }

    return out;
}

void JsonReader::FillCatalogue(const json::Node& doc) {
    for (auto& node : doc.AsArray()) {
        json::Dict map = node.AsDict();

        if (map.at("type") == json::Node{ "Stop"s }) {
            const auto& distances = CreateDistanceMap(map["road_distances"].AsDict());
            std::string currentStop = map["name"].AsString();
            catalogue_.AddStop(currentStop, { map["latitude"].AsDouble(), map["longitude"].AsDouble() });
            for (auto& [stop_name, distance] : distances) {
                catalogue_.SetDistance(currentStop, stop_name, distance);
            }
        }
        else if (map.at("type") == json::Node{ "Bus"s }) {
            auto stops = std::move(CreateRoute(map["stops"]));
            catalogue_.AddBus(map["name"].AsString(), stops, map["is_roundtrip"].AsBool());
        }
    }
}

json::Node::Value JsonReader::GetStopInfo(const json::Node& node_map) {
    const json::Dict& in = node_map.AsDict();
    json::Dict out;
    json::Array busses;

    auto stop = catalogue_.GetStop(in.at("name").AsString());
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

json::Node::Value JsonReader::GetBusInfo(const json::Node& node_map) {
    const auto& bus = catalogue_.GetBusStat(node_map.AsDict().at("name").AsString());
    json::Dict out;
    auto  id = node_map.AsDict().at("id");
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

json::Node::Value JsonReader::GetMapInfo(const json::Node& node_map) {
    json::Dict out;
    out["request_id"] = node_map.AsDict().at("id");

    std::ostringstream outstream;
    renderer_.RenderMap(*catalogue_.GetBuses(), outstream);
    std::string str = outstream.str();
    out["map"] = outstream.str();
    return out;
}

json::Node::Value JsonReader::GetRouteInfo(const json::Node& node_map) {//////////////////////////////////////////////////
    auto& map = node_map.AsDict();
    json::Builder bldr;
    auto opt = router_.GetRouteInfo(map.at("from"s).AsString(), map.at("to"s).AsString());

    if (!opt.has_value()) {
        bldr.StartDict().Key("request_id"s).Value(map.at("id"s).AsInt())
            .Key("error_message"s).Value("not found"s)
            .EndDict();
    }
    else {
        auto [weight, edges] = opt.value();
        auto arr = bldr.StartDict().Key("request_id"s).Value(map.at("id"s).AsInt())
            .Key("total_time"s).Value(weight)
            .Key("items"s).StartArray();

        for (auto edge : edges) {
            if (edge.isWait) {
                arr.StartDict()
                    .Key("type"s).Value("Wait"s)
                    .Key("stop_name"s).Value(std::string{ edge.name.data() })
                    .Key("time"s).Value(edge.weight)
                    .EndDict();
            }
            else {

                arr.StartDict()
                    .Key("type"s).Value("Bus"s)
                    .Key("bus"s).Value(std::string{ edge.name.data() })
                    .Key("span_count"s).Value(edge.span)
                    .Key("time"s).Value(edge.weight)
                    .EndDict();
            }

        }
        arr.EndArray().EndDict();

    }
    return bldr.Build().GetValue();
}


void JsonReader::PrintStat(const json::Node& arr, std::ostream& out) {
    if (arr.AsArray().empty()) { return; }
    json::Builder bldr;
    auto vctr = bldr.StartArray();

    for (auto& elem : arr.AsArray())
    {
        std::string type = elem.AsDict().at("type").AsString();
        if (type == "Stop") {
            vctr.Value(GetStopInfo(elem));
        }
        else if (type == "Bus") {
            vctr.Value(GetBusInfo(elem));
        }
        else if (type == "Map") {
            vctr.Value(GetMapInfo(elem));
        }
        else if (type == "Route") {
            vctr.Value(GetRouteInfo(elem));
        }
    }
    vctr.EndArray();
    json::Print(json::Document{bldr.Build()}, out);
}

svg::Color JsonReader::MakeColor(const json::Node& node) {

    if (node.IsArray()) {
        uint8_t r = static_cast<uint8_t>(node.AsArray().at(0).AsInt());
        uint8_t g = static_cast<uint8_t>(node.AsArray().at(1).AsInt());
        uint8_t b = static_cast<uint8_t>(node.AsArray().at(2).AsInt());
        if (node.AsArray().size() == 3) {
            return svg::Rgb({ r,g,b });
        }
        else
        {
            double a = node.AsArray().at(3).AsDouble();
            return svg::Rgba(r, g, b, a);
        }
    }
    else {
        return node.AsString();
    }

}

void JsonReader::SetRenderSettings(const json::Node& node_map) {
    const json::Dict& map = node_map.AsDict();

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

    renderer_.SetHeight(height)
        .SetWidth(width)
        .SetPadding(padding)
        .SetLineWidth(line_width)
        .SetStopRadius(stop_radius)
        .SetBusLabelFontSize(bus_label_font_size)
        .SetBusLabelOffset(bus_label_offset)
        .SetStopLabelFontSize(stop_label_font_size)
        .SetStopLabelOffset(stop_label_offset);

    svg::Color underlayer_color = MakeColor(map.at("underlayer_color"));
    renderer_.SetUnderlayerColor(underlayer_color).SetUnderlayerWidth(map.at("underlayer_width").AsDouble());

    std::vector<svg::Color> color_palette;
    for (auto v : map.at("color_palette").AsArray())
    {
        color_palette.push_back(MakeColor(v));
    }
    renderer_.SetColorPalette(color_palette);
}

void JsonReader::SetRouteSettings(const json::Node& node_map) {
    auto& map = node_map.AsDict();
    int bus_wait_time = map.at("bus_wait_time").AsInt();
    int bus_velocity = (map.at("bus_velocity").AsInt());
    router_.SetRoutingSettings(bus_wait_time, bus_velocity);
}


void JsonReader::ProcessRequest(const json::Document& doc, std::ostream& out) {
    auto& map = doc.GetRoot().AsDict();
    auto base_requests = map.find("base_requests");
    auto render_settings = map.find("render_settings");
    auto routing_settings = map.find("routing_settings");
    auto stat_requests = map.find("stat_requests");

    if (base_requests != map.end()) {
        FillCatalogue(base_requests->second);
    }
    if (render_settings != map.end()) {
        SetRenderSettings(render_settings->second);
    }
    if (routing_settings != map.end()) {
        SetRouteSettings(routing_settings->second);
    }
    if (stat_requests != map.end()) {
        PrintStat(stat_requests->second, out);
    }
}
