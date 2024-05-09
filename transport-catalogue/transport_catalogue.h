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
	double curvature;
};

class TransportCatalogue {
public:
	void AddStop(Stop& stop);

	void AddStop(const std::string& id, const geo::Coordinates& coordinates, const std::unordered_map<std::string_view, size_t>& stop_distance);

	void AddBus(const std::string& id, const std::vector<std::string_view>& route);

	std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

	const  Bus* GetBus(const std::string_view& stop_name)const;

	const Stop* GetStop(const std::string_view& stop_name)const;

	void SetDistance(const Stop* lhstop, const Stop* rhstop, size_t length);

	const size_t GetDistance(const Stop* lhstop, const Stop* rhstop) const;

private:

	void CheckExistanceAndCreateEmptyStop(std::string std);

	struct StopPairHasher {
		std::size_t operator()(const std::pair<const Stop*, const Stop*>& stopPair) const {
			std::hash<const void*> stopHasher;
			std::size_t firstHash = stopHasher(stopPair.first);
			std::size_t secondHash = stopHasher(stopPair.second);
			return firstHash ^ secondHash;
		}
	};

	std::deque<Stop> deque_stops_;
	std::unordered_map <std::string_view, Stop*> stops_;
	std::deque<Bus> deque_buses_;
	std::unordered_map<std::string_view, Bus*> buses_;

	std::unordered_map <const std::pair<const Stop*, const Stop*>, size_t, StopPairHasher> distances_;
};