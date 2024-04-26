#pragma once
#include "geo.h"
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Stop
{
	geo::Coordinates coordinates{ 0.0,0.0 };
	std::set<std::string> buses;

	bool operator==(const Stop& other) const {
		return (buses == other.buses) && (coordinates == other.coordinates);
	}
};

struct Bus
{
	std::vector<std::string> stops{};
	
	bool operator==(const Bus& other) const {
		return stops == other.stops;
	}
};

class TransportCatalogue {
public:
	void AddStop(const std::string& id, const geo::Coordinates& coordinates);

	void AddBus(const std::string& id, const std::vector<std::string_view>& route);

	std::string CheckBus(const std::string_view& bus_name)const;
	
	std::string CheckStop(const std::string_view& stop_name)const;

private:
	std::unordered_map <std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;

};