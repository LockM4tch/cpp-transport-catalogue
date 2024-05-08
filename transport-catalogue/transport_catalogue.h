#pragma once
#include "geo.h"
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Stop {
	std::string stop_name;
	geo::Coordinates coordinates{ 0.0,0.0 };
	std::unordered_map<std::string_view, uint32_t> stop_distance;

	std::set<std::string> buses;

	bool operator==(const Stop& other) const {
		return (buses == other.buses) && (coordinates == other.coordinates);
	}
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> stops;

	bool operator==(const Bus& other) const {
		return stops == other.stops;
	}
};

struct BusStat {
	size_t stops_on_route;
	size_t unique_stops;
	size_t route_length;
	float curvature;
};

class TransportCatalogue {
public:
	void AddStop(const std::string& id, const geo::Coordinates& coordinates, const std::unordered_map<std::string_view, uint32_t>& stop_distance);

	void AddBus(const std::string& id, const std::vector<std::string_view>& route);

	std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

	const  Bus* GetBus(const std::string_view& stop_name)const;

	const Stop* GetStop(const std::string_view& stop_name)const;

private:

	const int GetDistance(Stop* lhstop, Stop* rhstop) const;

	std::deque<Stop> deque_stops_;
	std::unordered_map <std::string_view, Stop*> stops_;
	std::deque<Bus> deque_buses_;
	std::unordered_map<std::string_view, Bus*> buses_;

};