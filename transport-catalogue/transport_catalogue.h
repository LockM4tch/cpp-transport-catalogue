#pragma once
#include "domain.h"
#include "geo.h"
#include "graph.h"
#include "router.h"

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

struct BusStat {
	size_t stops_on_route;
	size_t unique_stops;
	size_t route_length;
	double curvature;
};

struct Edges {
	double total_weight = 0;
	std::vector<graph::Edge<double>> edges;
};

class TransportCatalogue {
public:
	using Weight = double;
	using Graph = graph::DirectedWeightedGraph<Weight>;

	TransportCatalogue() = default;

	void AddStop(const Stop& stop);

	void AddStop(const std::string& id, const geo::Coordinates& coordinates);

	void AddBus(const std::string& id, const std::vector<std::string_view>& route, bool isRoundtrip);

	std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

	const  Bus* GetBus(const std::string_view& stop_name)const;

	const std::deque<Bus>* GetBuses()const;

	const Stop* GetStop(const std::string_view& stop_name)const;

	void SetDistance(std::string_view lhstop, std::string_view rhstop, size_t length);
	void SetDistance(const Stop* lhstop, const Stop* rhstop, size_t length);
	size_t GetDistance(const Stop* lhstop,const Stop* rhstop) const;

	void SetRoutingSettings(const int bus_wait_time, const int bus_velocity);
	std::optional<Edges> GetRouteInfo(const Stop* from, const Stop* to);
	std::optional<Edges> GetRouteInfo(const std::string_view from, const std::string_view to);



private:

	void CreateGraph();
	void AddStopIfMissing(const std::string& std);

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
	
	std::unique_ptr<Graph> graph_ = nullptr;
	std::unique_ptr<graph::Router<Weight>> router_ = nullptr;

	int bus_wait_time_ = -1;
	int bus_velocity_ = -1;

	std::unordered_map <const std::pair<const Stop*, const Stop*>, size_t, StopPairHasher> distances_;
};
