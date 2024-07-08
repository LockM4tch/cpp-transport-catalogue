#pragma once
#include "graph.h"
#include "router.h"

#include <memory>

using Weight = double;
using Graph = graph::DirectedWeightedGraph<Weight>;


struct Edges {
	double total_weight = 0;
	std::vector<graph::Edge<double>> edges;
};


class TransportRouter {
public:
	TransportRouter();
	~TransportRouter();
private:
	std::unique_ptr<Graph> graph_ = nullptr;
	std::unique_ptr<graph::Router<Weight>> router_ = nullptr;

	int bus_wait_time_ = -1;
	int bus_velocity_ = -1;

};
