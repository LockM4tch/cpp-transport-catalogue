#pragma once
#include "input_reader.h"
#include "json.h"
#include "json_reader.h"
#include "request_handler.h"
#include "stat_reader.h"
#include "svg.h"
#include "transport_catalogue.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <string>


void test() {
	using namespace std;
	    const double WIDTH = 600.0;
	    const double HEIGHT = 400.0;
	    const double PADDING = 50.0;
	
		TransportCatalogue catalogue;
		json::Document jsn = json::Load(std::cin);
		json_reader::ProcessRequest(jsn, catalogue, std::cout);
	
		//std::deque<const Bus*> buses;
		//for (auto& bus : *catalogue.GetBuses()) {
		//	buses.push_back(&bus);
		//}


		//std::sort(buses.begin(), buses.end(), [](const Bus* lhs, const Bus* rhs) {return lhs->bus_name < rhs->bus_name; });
		//vector<geo::Coordinates> geo_coords;

		//
		//for (auto& bus : buses) {
		//	for (auto& s : bus->stops) {
		//		geo_coords.push_back(s->coordinates);
		//	}
		//}

	 //   // Создаём проектор сферических координат на карту
	 //   const SphereProjector proj{
	 //       geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
	 //   };
		//
		//std::unordered_map<geo::Coordinates, svg::Point> scrinpoints_;

	 //   // Проецируем и выводим координаты
	 //   for (const auto& geo_coord : geo_coords) {
	 //       const svg::Point screen_coord = proj(geo_coord);
		//	scrinpoints_[geo_coord] = screen_coord;
	 //   }

		//svg::Document doc;
		//for (auto bus : buses) {
		//	auto stops = bus->stops;
		//	svg::Polyline route;
		//	for (auto stop : stops) {
		//		 route.AddPoint(scrinpoints_.at(stop->coordinates));
		//	}
		//	doc.Add(route);
		//}
		//doc.Render(std::cout);
}