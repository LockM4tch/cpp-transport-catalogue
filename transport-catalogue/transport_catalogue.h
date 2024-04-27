#pragma once
#include "geo.h"
#include <iostream>
#include <set>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <vector>

struct Stop
{
	std::string stop_name; //добавил имя остановки, однако считаю что использование ключа в map в качестве имени будет более рационально, дабы не плодить лишних сущностей
	geo::Coordinates coordinates{ 0.0,0.0 };
	std::set<std::string> buses;

	bool operator==(const Stop& other) const {
		return (buses == other.buses) && (coordinates == other.coordinates);
	}
};

struct Bus
{
	std::string bus_name;
	std::vector<std::string> stops{};
	
	bool operator==(const Bus& other) const {
		return stops == other.stops;
	}
};

class TransportCatalogue {
public:
	void AddStop(const std::string& id, const geo::Coordinates& coordinates);

	void AddBus(const std::string& id, const std::vector<std::string_view>& route);

	std::tuple<int, int, float> GetBusInfo(const std::string_view& bus_name)const;
	
	const std::pair <const std::string, Bus>* Get_Bus_By_Name(const std::string_view& stop_name)const;

	const std::pair <const std::string, Stop>* Get_Stop_By_Name(const std::string_view& stop_name)const;

private:
	std::unordered_map <std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;

};