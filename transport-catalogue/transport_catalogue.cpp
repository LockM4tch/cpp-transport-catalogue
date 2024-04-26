#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string& id, geo::Coordinates coordinates) {
	if (stops_.count(id) == 0) {
		stops_[id] = {coordinates,{} };
	}
	else { stops_[id].coordinates = coordinates; }
}

void TransportCatalogue::AddBus(std::string& id, std::vector<std::string_view> route) {
	std::vector<std::string> route_str;
	for (auto& sv : route) {
		route_str.push_back(std::string(sv));
		stops_[std::string(sv)].buses.insert(id);
	}
		buses_.insert({ id , route_str });
}

std::string TransportCatalogue::CheckBus(std::string_view bus_name)const {

	auto bus_it = buses_.find({ bus_name.data(), {" "} });

	if (bus_it == buses_.end()) {
		return "not found";
	}
	else {
		int stops_on_route; float route_length = 0;
		std::unordered_set<std::string> unique_stops;

		stops_on_route = bus_it->stops.size(); 

		for (int i = 1; i < stops_on_route; ++i) {
			unique_stops.insert(bus_it->stops.at(i - 1));
			route_length += ComputeDistance(stops_.find(bus_it->stops.at(i - 1))->second.coordinates, stops_.find(bus_it->stops.at(i))->second.coordinates);
		}
		std::ostringstream out;
		out << stops_on_route << " stops on route, " << unique_stops.size() << " unique stops, " << route_length << " route length";
		return out.str();
	}

}

std::string TransportCatalogue::CheckStop(std::string_view stop_name)const {
		
	auto current_stop = stops_.find(stop_name.data());
	//Stop Samara : not found
	if (current_stop == stops_.end()) { return "not found"; }
	
	//Stop Prazhskaya : no buses
	if (current_stop->second.buses.empty()) { return "no buses"; }

	//Stop Biryulyovo Zapadnoye : buses 256 828
	std::ostringstream out;
	out << "buses";
	for (auto bus : current_stop->second.buses) {
		out << " " << bus;
	}
	return out.str();
}
