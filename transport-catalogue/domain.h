#pragma once
#include "geo.h"
#include  <string>
#include  <set>
#include  <vector>

struct Stop {
	std::size_t id = 0;
	std::string stop_name;
	geo::Coordinates coordinates{ 0.0,0.0 };

	std::set<std::string> buses;

	bool operator==(const Stop& other) const {
		return (buses == other.buses) && (coordinates == other.coordinates);
	}
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> stops;
	bool isRoundtrip = false;
	bool operator==(const Bus& other) const {
		return stops == other.stops;
	}
};
