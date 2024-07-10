#pragma once
#include <memory>
#include <deque>

#include "graph.h"
#include "router.h"
#include "domain.h"
#include "transport_catalogue.h"


	using Weight = double;

struct Edges {
	double total_weight = 0;
	std::vector<graph::Edge<Weight>> edges;
};

class TransportRouter{
	using Graph = graph::DirectedWeightedGraph<Weight>;
public:
	TransportRouter(TransportCatalogue& catalogue);

	~TransportRouter() = default;
	void SetRoutingSettings(const int bus_wait_time, const int bus_velocity);

	std::optional<Edges> GetRouteInfo(const std::string_view from, const std::string_view to);
	std::optional<Edges> GetRouteInfo(const Stop* from, const Stop* to);

private:
		void CreateGraph();

	std::unique_ptr<Graph> graph_ = nullptr;
	std::unique_ptr <graph::Router<Weight>> router_ = nullptr;
	TransportCatalogue& catalogue_;

	int bus_wait_time_ = -1;
	int bus_velocity_ = -1;


};
