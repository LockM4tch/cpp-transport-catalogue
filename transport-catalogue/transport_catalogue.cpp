#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& id, const geo::Coordinates& coordinates = {0,0}) {
	if (stops_.count(id) == 0) {
		deque_stops_.push_back({id, coordinates, {}	});
		stops_[deque_stops_.back().stop_name] = &(deque_stops_.back());
	}
	else { stops_[id]->coordinates = coordinates; }
}

void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& route) {
	std::vector<Stop*> route_str;
	if (buses_.count(id) == 0) {
		deque_buses_.push_back({ id, {} });
		Bus* bus = &deque_buses_.back();
		buses_[bus->bus_name] = bus;
	}

	for (auto& sv : route) {
		if (stops_.find(sv) == stops_.end()) { 
			AddStop(std::string(sv)); 
		}
		route_str.push_back(stops_[sv]);
		stops_[sv]->buses.insert(id);
	}
	buses_[id]->stops = route_str;
}

std::optional<BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const {
	auto bus_it = buses_.find(bus_name.data());

	if (bus_it == buses_.end()) {
		return std::optional<std::tuple<int, size_t, float>>();
	}
	else {
		int stops_on_route = bus_it->second->stops.size(); ;
		float route_length = 0;
		std::unordered_set<std::string> unique_stops;

		for (int i = 1; i < stops_on_route; ++i) {
			auto& stop_first = bus_it->second->stops.at(i - 1);
			auto& stop_second = bus_it->second->stops.at(i);

			unique_stops.insert(stop_first->stop_name);
			route_length += geo::ComputeDistance(stop_first->coordinates, stop_second->coordinates);
		}
		return std::optional<BusStat> ({ stops_on_route, unique_stops.size(), route_length });
	}
}

const Bus* TransportCatalogue::GetBus(const std::string_view& bus_name)const {
	auto bus_iter = buses_.find(bus_name);
	if (bus_iter == buses_.end()) {
		return {nullptr};
	}
	return bus_iter->second;
}

const Stop* TransportCatalogue::GetStop(const std::string_view& stop_name)const {
	auto stop_iter = stops_.find(stop_name);
	if (stop_iter == stops_.end()) {
		return {nullptr};
	}
	return stop_iter->second;
}
