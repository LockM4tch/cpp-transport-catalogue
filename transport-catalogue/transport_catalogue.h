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

struct Bus;

struct Stop
{
	//std::string name_;
	geo::Coordinates coordinates{ 0.0,0.0 };
	std::set<std::string> buses;

	bool operator==(const Stop& other) const {
		return (buses == other.buses) && (coordinates == other.coordinates);
	}
};

struct Bus
{
	std::string name_;
	std::vector<std::string> stops{};
	
	bool operator==(const Bus& other) const {
		return name_ == other.name_;
	}
};

struct BusHasher {
	std::size_t operator()(const Bus& bus) const {
		return std::hash<std::string>()(bus.name_);
	}
};

class TransportCatalogue {
public:
	void AddStop(std::string& id, geo::Coordinates coordinates);

	void AddBus(std::string& id, std::vector<std::string_view> route);

	std::string CheckBus(std::string_view bus_name)const;
	
	std::string CheckStop(std::string_view stop_name)const;

private:
	std::unordered_map <std::string, Stop> stops_;
	std::unordered_set<Bus, BusHasher> buses_;

};