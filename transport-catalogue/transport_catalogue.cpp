#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& stop_name, const geo::Coordinates& coordinates = {},
	const std::unordered_map<std::string_view, uint32_t>& stop_distance = {}) {
	if (stops_.count(stop_name) == 0) {
		deque_stops_.push_back({ stop_name, coordinates, stop_distance, {} });
		Stop& new_stop = deque_stops_.back();
		stops_[new_stop.stop_name] = &(new_stop);
	}
	if (coordinates != geo::Coordinates{}) {
		stops_[stop_name]->coordinates = coordinates;
	}
	if (!stop_distance.empty()) {
		stops_[stop_name]->stop_distance = stop_distance;
	}
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
		return std::optional<BusStat>();
	}
	else {
		size_t stops_on_route = bus_it->second->stops.size(); ;
		size_t route_length = 0;
		double ideal_distance = 0.;
		std::unordered_set<std::string> unique_stops;


		for (int i = 1; i < stops_on_route; ++i) {
			auto& stop_first = bus_it->second->stops.at(i - 1);
			auto& stop_second = bus_it->second->stops.at(i);

			unique_stops.insert(stop_first->stop_name);

			route_length += GetDistance(stop_first, stop_second);
			ideal_distance += geo::ComputeDistance(stop_first->coordinates, stop_second->coordinates);
		}

		return { stops_on_route, unique_stops.size(), route_length , (route_length / ideal_distance) };
	}
}

const Bus* TransportCatalogue::GetBus(const std::string_view& bus_name)const {
	auto bus_iter = buses_.find(bus_name);
	if (bus_iter == buses_.end()) {
		return nullptr;
	}
	return bus_iter->second;
}

const Stop* TransportCatalogue::GetStop(const std::string_view& stop_name)const {
	auto stop_iter = stops_.find(stop_name);
	if (stop_iter == stops_.end()) {
		return nullptr;
	}
	return stop_iter->second;
}

const int TransportCatalogue::GetDistance(Stop* from, Stop* to) const {
	if (from->stop_distance.count(to->stop_name) > 0)
	{
		return	from->stop_distance.at(to->stop_name);
	}
	else {
		return to->stop_distance.at(from->stop_name);
	}
}