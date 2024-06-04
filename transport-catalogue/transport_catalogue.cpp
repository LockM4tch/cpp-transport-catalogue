#include "transport_catalogue.h"


void TransportCatalogue::AddStop(const std::string& stop_name, const geo::Coordinates& coordinates = {}
){
	if (stops_.count(stop_name) == 0) {
		deque_stops_.push_back({ stop_name, coordinates, {} });
		Stop& new_stop = deque_stops_.back();
		stops_[new_stop.stop_name] = &(new_stop);
		return;
	}

	if (coordinates != geo::Coordinates{}) {
		stops_[stop_name]->coordinates = coordinates;
	}
}

void TransportCatalogue::AddStop(Stop& stop) {
	AddStop(stop.stop_name, stop.coordinates);
}

void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& route, bool isRoundtrip) {
	std::vector<Stop*> route_str;
	if (buses_.count(id) == 0) {
		deque_buses_.push_back({ id, {}, isRoundtrip });
		Bus* bus = &deque_buses_.back();
		buses_[bus->bus_name] = bus;
	}

	for (auto& sv : route) {
		AddStopIfMissing(std::string(sv));
		route_str.push_back(stops_[sv]);
		stops_[sv]->buses.insert(id);
	}
	buses_[id]->stops = route_str;
}

void TransportCatalogue::AddStopIfMissing(const std::string& std) {
	if (stops_.find(std) == stops_.end()) {
		AddStop(std);
	}
}

std::optional<BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const {
	auto bus_it = buses_.find(bus_name.data());
	if (bus_it == buses_.end()) {
		return std::optional<BusStat>();
	}
	else {
		std::vector<Stop*> route = bus_it->second->stops;
		if (!bus_it->second->isRoundtrip) {
			for (int i = static_cast<int>(route.size()) - 2; i >= 0; --i) {
				route.push_back(route.at(i));
			}
		}
		size_t stops_on_route = route.size();
		size_t route_length = 0;
		double ideal_distance = 0.;
		std::unordered_set<std::string> unique_stops;


		for (size_t i = 1; i < stops_on_route; ++i) {
			auto& stop_first = route.at(i - 1);
			auto& stop_second = route.at(i);

			unique_stops.insert(stop_first->stop_name);

			route_length += GetDistance(stop_first, stop_second);
			ideal_distance += geo::ComputeDistance(stop_first->coordinates, stop_second->coordinates);
		}
		ideal_distance = route_length / ideal_distance;

		BusStat st{ stops_on_route, unique_stops.size(), route_length , ideal_distance};
		return st;
	}
}

const Bus* TransportCatalogue::GetBus(const std::string_view& bus_name)const {
	auto bus_iter = buses_.find(bus_name);
	if (bus_iter == buses_.end()) {
		return nullptr;
	}
	return bus_iter->second;
}

const std::deque<Bus>* TransportCatalogue::GetBuses()const {
	return &deque_buses_;
}

const Stop* TransportCatalogue::GetStop(const std::string_view& stop_name)const {
	auto stop_iter = stops_.find(stop_name);
	if (stop_iter == stops_.end()) {
		return nullptr;
	}
	return stop_iter->second;
}

 size_t TransportCatalogue::GetDistance(const Stop* from,const Stop* to) const {
	if (distances_.count({ from, to }) != 0)
	{
		return	distances_.at({ from,to});
	}
	else{
		return	distances_.at({to, from});
	}
}

 void TransportCatalogue::SetDistance(std::string_view lhstop, std::string_view rhstop, size_t length) {
	 AddStopIfMissing(std::string(rhstop));
	 distances_[std::pair{ GetStop(lhstop),  GetStop(rhstop) }] = length;
	 
 }
void TransportCatalogue::SetDistance(const Stop* lhstop,const Stop* rhstop, size_t length) {
	distances_[std::pair{lhstop, rhstop}] = length;
}
