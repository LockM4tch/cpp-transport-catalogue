#pragma once

#include "svg.h"
#include "geo.h"
#include "domain.h"

#include <algorithm>
#include <deque>
#include <map>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <vector>

inline const double EPSILON = 1e-6;

bool IsZero(double value);

template <>
struct std::hash<geo::Coordinates> {
    size_t operator()(const geo::Coordinates& k) const {
        return std::hash<double>()(k.lat) ^ std::hash<double>()(k.lng);
    }
};

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

class MapRenderer
{
public:
    MapRenderer& SetWidth(double width);
    MapRenderer& SetHeight(double height);
    MapRenderer& SetPadding(double padding);
    MapRenderer& SetLineWidth(double line_width);
    MapRenderer& SetStopRadius(double stop_radius);
    MapRenderer& SetBusLabelFontSize(int font_size);
    MapRenderer& SetBusLabelOffset(svg::Point offset);
    MapRenderer& SetStopLabelFontSize(int font_size);
    MapRenderer& SetStopLabelOffset(svg::Point offset);
    MapRenderer& SetUnderlayerColor(svg::Color color);
    MapRenderer& SetUnderlayerWidth(double width);
    MapRenderer& SetColorPalette(const std::vector<svg::Color>& colors);

    void DrawRoutes(std::vector<const Bus*>& buses, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc);
    void MakeNameText(svg::Text& nameText, const svg::Point& point, bool isUnderlayer = false);
    svg::Text MakeBusNameText(const std::string& name, const svg::Point& point, const svg::Color& color, bool isUnderlayer);
    svg::Text MakeStopNameText(const std::string& name, const svg::Point& point, bool isUnderlayer);
    void DrawRouteNames(std::vector<const Bus*>& buses, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc);
    void DrawStops(std::map<std::string, const Stop*>& stops, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc);
    void DrawStopNames(std::map<std::string, const Stop*>& stops, std::unordered_map<geo::Coordinates, svg::Point>& scrinpoints_, svg::Document& doc);
    void RenderMap(const std::deque<Bus>& buses_in, std::ostream& out);

private:
    double width_ = 600;
    double height_ = 400;
    double padding_ = 50;
    double line_width_ = 50;
    double stop_radius_ = 5;
    int bus_label_font_size_ = 14;
    svg::Point bus_label_offset_ = { 5,15 };
    int	stop_label_font_size_ = 20;
    svg::Point stop_label_offset_ = { 7, -3 };
    svg::Color underlayer_color_ = svg::Rgba{ 255,255,255,0.85 };
    double underlayer_width_ = 3;
    std::vector<svg::Color> color_palette_ = { "green", svg::Rgb{255,160,0} , "red" };
};
