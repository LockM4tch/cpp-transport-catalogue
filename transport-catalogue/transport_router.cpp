#include "transport_router.h"

TransportRouter::TransportRouter(TransportCatalogue& catalogue) : catalogue_(catalogue){}


void TransportRouter::SetRoutingSettings(const int bus_wait_time, const int bus_velocity) {
	bus_wait_time_ = bus_wait_time;
	bus_velocity_ = bus_velocity;
}

void TransportRouter::CreateGraph(){
	if (graph_ != nullptr) { delete graph_.release(); }
	auto all_stops = catalogue_.GetStops();
	size_t graph_size = all_stops->size();
	graph_ = std::make_unique<Graph>(std::move(Graph(graph_size * 2)));
	for (size_t i = 0; i < graph_size; i++) {
		graph_->AddEdge({ i, graph_size + i, static_cast<double>(bus_wait_time_), true, all_stops->at(i).stop_name });
	}
	for (auto& bus : *catalogue_.GetBuses()) {
		auto stops = bus.stops;

		auto addEdges = [&](auto begin, auto end) {
			for (auto it = begin; it != end; ++it) {
				auto stop_one = *it;
				double distance = 0;
				int span = 0;
				for (auto it2 = it + 1; it2 != end; ++it2) {
					auto stop_two = *it2;
					distance += catalogue_.GetDistance(*(it2 - 1), stop_two);
					graph::Edge<double> edge{ stop_one->id + graph_size, stop_two->id, (distance / 1000 / bus_velocity_ * 60), false, bus.bus_name, ++span };
					graph_->AddEdge(edge);
				}
			}
			};
		addEdges(stops.begin(), stops.end());
		if (!bus.isRoundtrip) {
			addEdges(stops.rbegin(), stops.rend());
		}
	}
}

std::optional<Edges> TransportRouter::GetRouteInfo(const std::string_view from, const std::string_view to) {
	return GetRouteInfo(catalogue_.GetStop(from), catalogue_.GetStop(to));
}

std::optional<Edges> TransportRouter::GetRouteInfo(const Stop* from, const Stop* to) {
	if (graph_ == nullptr) { CreateGraph(); }
	if (router_ == nullptr) {
		graph::Router router(*graph_);
		router_ = std::make_unique<graph::Router<Weight>>(router);
	}
	auto route = router_->BuildRoute(from->id, to->id);
	if (!route.has_value()) {
		return std::nullopt;
	}
	std::vector<size_t> edgesId = route.value().edges;
	std::vector < graph::Edge<Weight>> edges;
	for (auto edgeId : edgesId) {
		edges.push_back(graph_->GetEdge(edgeId));
	}
	return Edges{ std::move(route.value().weight), std::move(edges) };
}
