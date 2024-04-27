#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string& id, const geo::Coordinates& coordinates) {
	if (stops_.count(id) == 0) {
		stops_[id] = { id, coordinates,{} };
	}
	else { stops_[id].coordinates = coordinates; }
}

void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& route) {
	std::vector<std::string> route_str;
	for (auto& sv : route) {
		route_str.push_back(std::string(sv));
		stops_[std::string(sv)].buses.insert(id);
	}
	buses_[id].stops = route_str;
}

const std::pair<const std::string, Bus>* TransportCatalogue::Get_Bus_By_Name(const std::string_view& bus_name)const {
	auto bus_iter = buses_.find(bus_name.data());
	if (bus_iter == buses_.end()) {
		return {nullptr};
	}
	return &(*bus_iter);
}

const std::pair<const std::string,Stop>* TransportCatalogue::Get_Stop_By_Name(const std::string_view& stop_name)const {
	auto stop_iter = stops_.find(stop_name.data());
	if (stop_iter == stops_.end()) {
		return {nullptr};
	}
	return &(*stop_iter);
}
