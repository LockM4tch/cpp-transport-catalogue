#include "map_renderer.h"

bool IsZero(double value){
     return std::abs(value) < EPSILON;
}

MapRenderer& MapRenderer::SetWidth(double width) {
    width_ = width;
    return *this;
}
MapRenderer& MapRenderer::SetHeight(double height) {
    height_ = height;
    return *this;
}
MapRenderer& MapRenderer::SetPadding(double padding) {
    padding_ = padding;
    return *this;
}
MapRenderer& MapRenderer::SetLineWidth(double line_width) {
    line_width_ = line_width;
    return *this;
}
MapRenderer& MapRenderer::SetStopRadius(double stop_radius) {
    stop_radius_ = stop_radius;
    return *this;
}
MapRenderer& MapRenderer::SetBusLabelFontSize(int font_size) {
    bus_label_font_size_ = font_size;
    return *this;
}
MapRenderer& MapRenderer::SetBusLabelOffset(svg::Point offset) {
    bus_label_offset_ = offset;
    return *this;
}
MapRenderer& MapRenderer::SetStopLabelFontSize(int font_size) {
    stop_label_font_size_ = font_size;
    return *this;
}
MapRenderer& MapRenderer::SetStopLabelOffset(svg::Point offset) {
    stop_label_offset_ = offset;
    return *this;
}
MapRenderer& MapRenderer::SetUnderlayerColor(svg::Color color) {
    underlayer_color_ = color;
    return *this;
}
MapRenderer& MapRenderer::SetUnderlayerWidth(double width) {
    underlayer_width_ = width;
    return *this;
}
MapRenderer& MapRenderer::SetColorPalette(const std::vector<svg::Color>& colors) {
    color_palette_ = colors;
    return *this;
}



void MapRenderer::DrawRoutes(std::vector<const Bus*>& buses, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc){
    int color_counter = 0;
    for (auto bus : buses) {
        auto stops = bus->stops;
        svg::Polyline route;
        route.SetStrokeWidth(line_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        route.SetStrokeColor(color_palette_.at(color_counter)).SetFillColor("none");
        for (auto stop : stops) {
            route.AddPoint(scrinpoints_.at(stop->coordinates));
        }
        
        if (!bus->isRoundtrip) {
            for (int i = static_cast<int>(stops.size()) - 2; i >= 0; --i) {
                route.AddPoint(scrinpoints_.at(stops.at(i)->coordinates));
            }
        }

        doc.Add(route);
        if (++color_counter; color_counter == static_cast<int>(color_palette_.size())) {
            color_counter = 0;
        }
    }

}

void MapRenderer::MakeNameText(svg::Text& nameText, const svg::Point& point, bool isUnderlayer) {
    nameText.SetPosition(point).SetFontFamily("Verdana");
    if (isUnderlayer) {
        nameText
            .SetFillColor(underlayer_color_)
            .SetStrokeColor(underlayer_color_)
            .SetStrokeWidth(underlayer_width_)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    }
}

svg::Text MapRenderer::MakeStopNameText(const std::string& name, const svg::Point& point, bool underlayer = false) {
    svg::Text nameText;
    nameText
        .SetFillColor("black")
        .SetOffset(stop_label_offset_)
        .SetFontSize(stop_label_font_size_)
        .SetData(name);
    MakeNameText(nameText, point, underlayer);
    return nameText;
}

svg::Text MapRenderer::MakeBusNameText(const std::string& name, const svg::Point& point,const svg::Color& color, bool underlayer = false) {
    svg::Text nameText;
    nameText
            .SetFillColor(color)
            .SetOffset(bus_label_offset_)
            .SetFontSize(bus_label_font_size_)
            .SetFontWeight("bold")
            .SetData(name);
    MakeNameText(nameText, point, underlayer);
    return nameText;
}

void MapRenderer::DrawRouteNames(std::vector<const Bus*>& buses, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc) {
    const bool isUnderlayer = true;
    auto currentColor = color_palette_.begin();
    
    for (auto bus : buses) {
        auto stops = bus->stops;
        
        svg::Point point = scrinpoints_.at(stops.front()->coordinates);
        svg::Color color = *currentColor;
        doc.Add(MakeBusNameText(bus->bus_name, point, color, isUnderlayer));
        doc.Add(MakeBusNameText(bus->bus_name, point, color));
        
        if (!bus->isRoundtrip && (stops.front() != stops.back())) {
           point = scrinpoints_.at(stops.back()->coordinates);
           doc.Add(MakeBusNameText(bus->bus_name, point, color, isUnderlayer));
           doc.Add(MakeBusNameText(bus->bus_name, point, color));
        }
        if (++currentColor; currentColor == color_palette_.end()) {
            currentColor = color_palette_.begin();
        }

    }

}

void MapRenderer::DrawStops(std::map<std::string, const Stop*>& stops, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc) {

        for (auto&[name, stop] : stops) {
        svg::Circle stopCircle;
        auto point = scrinpoints_.at(stop->coordinates);
        stopCircle.SetCenter(point)
            .SetRadius(stop_radius_)
            .SetFillColor("white");
        doc.Add(stopCircle);
    }

}

void MapRenderer::DrawStopNames(std::map<std::string, const Stop*>& stops, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc) {
    const bool isUnderlayer = true;
    for (auto& [name, stop] : stops) {
        auto point = scrinpoints_.at(stop->coordinates);
        doc.Add(MakeStopNameText(name, point, isUnderlayer));
        doc.Add(MakeStopNameText(name, point));
    }
}

void MapRenderer::RenderMap(const std::deque<Bus>& buses_in, std::ostream& out) {
    std::vector<const Bus*> buses;
    for (auto& bus : buses_in) {
        buses.push_back(&bus);
    }
    std::sort(buses.begin(), buses.end(), [](const Bus* lhs, const Bus* rhs) {return lhs->bus_name < rhs->bus_name; });

    std::vector<geo::Coordinates> geo_coords;
    std::map<std::string, const Stop*> activeStops;
    for (auto& bus : buses) {
        for (auto& s : bus->stops) {
            geo_coords.push_back(s->coordinates);
            activeStops[s->stop_name] = s;
        }
    }
    //удалить потор€ющиес€ координаты
    geo_coords.erase(std::unique(geo_coords.begin(), geo_coords.end()), geo_coords.end());

    // —оздаЄм проектор сферических координат на карту
    const SphereProjector proj{
        geo_coords.begin(), geo_coords.end(), width_, height_, padding_
    };

    //сохран€ем координаты дл€ быстрого конвертировани€
    std::unordered_map<geo::Coordinates, svg::Point> scrinpoints_;
    for (const auto& geo_coord : geo_coords) {
        scrinpoints_[geo_coord] = proj(geo_coord);
    }

    svg::Document doc;
    DrawRoutes(buses, scrinpoints_, doc);
    DrawRouteNames(buses, scrinpoints_, doc);  
    DrawStops(activeStops, scrinpoints_, doc);    
    DrawStopNames(activeStops, scrinpoints_, doc);  

    doc.Render(out);
}