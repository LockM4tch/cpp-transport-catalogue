#include "transport_catalogue.h"


void TransportCatalogue::AddStop(const std::string& stop_name, const geo::Coordinates& coordinates = {}){
	if (stops_.count(stop_name) == 0) {
		deque_stops_.push_back({ deque_stops_.size(), stop_name, coordinates, {} });
		Stop& new_stop = deque_stops_.back();
		stops_[new_stop.stop_name] = &(new_stop);
		return;
	}

	if (coordinates != geo::Coordinates{}) {
		stops_[stop_name]->coordinates = coordinates;
	}
}

void TransportCatalogue::AddStop(const Stop& stop) {
	AddStop(stop.stop_name, stop.coordinates);
}


void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string_view>& route, bool isRoundtrip) {
	std::vector<Stop*> route_str;
	if (buses_.count(id) == 0) {
		deque_buses_.push_back({ id, {}, isRoundtrip });
		Bus* bus = &deque_buses_.back();
		buses_[bus->bus_name] = bus;
	}

	for (auto& sv : route) {
		AddStopIfMissing(std::string(sv));
		route_str.push_back(stops_[sv]);
		stops_[sv]->buses.insert(id);
	}
	buses_[id]->stops = route_str;
}

void TransportCatalogue::AddStopIfMissing(const std::string& std) {
	if (stops_.find(std) == stops_.end()) {
		AddStop(std);
	}
}

std::optional<BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const {
	auto bus_it = buses_.find(bus_name.data());
	if (bus_it == buses_.end()) {
		return std::optional<BusStat>();
	}
	else {
		std::vector<Stop*> route = bus_it->second->stops;
		if (!bus_it->second->isRoundtrip) {
			for (int i = static_cast<int>(route.size()) - 2; i >= 0; --i) {
				route.push_back(route.at(i));
			}
		}
		size_t stops_on_route = route.size();
		size_t route_length = 0;
		double ideal_distance = 0.;
		std::unordered_set<std::string> unique_stops;


		for (size_t i = 1; i < stops_on_route; ++i) {
			auto& stop_first = route.at(i - 1);
			auto& stop_second = route.at(i);

			unique_stops.insert(stop_first->stop_name);

			route_length += GetDistance(stop_first, stop_second);
			ideal_distance += geo::ComputeDistance(stop_first->coordinates, stop_second->coordinates);
		}
		ideal_distance = route_length / ideal_distance;

		BusStat st{ stops_on_route, unique_stops.size(), route_length , ideal_distance};
		return st;
	}
}

const Bus* TransportCatalogue::GetBus(const std::string_view& bus_name)const {
	auto bus_iter = buses_.find(bus_name);
	if (bus_iter == buses_.end()) {
		return nullptr;
	}
	return bus_iter->second;
}

const std::deque<Bus>* TransportCatalogue::GetBuses()const {
	return &deque_buses_;
}

const Stop* TransportCatalogue::GetStop(const std::string_view& stop_name)const {
	auto stop_iter = stops_.find(stop_name);
	if (stop_iter == stops_.end()) {
		return nullptr;
	}
	return stop_iter->second;
}

 size_t TransportCatalogue::GetDistance(const Stop* from,const Stop* to) const {
	if (distances_.count({ from, to }) != 0)
	{
		return	distances_.at({ from,to});
	}
	else{
		return	distances_.at({to, from});
	}
}

 void TransportCatalogue::SetDistance(std::string_view lhstop, std::string_view rhstop, size_t length) {
	 AddStopIfMissing(std::string(rhstop));
	 SetDistance(GetStop(lhstop),  GetStop(rhstop), length);
	 
 }
void TransportCatalogue::SetDistance(const Stop* lhstop,const Stop* rhstop, size_t length) {
	distances_[std::pair{lhstop, rhstop}] = length;
	if (distances_.count(std::pair{ rhstop,lhstop }) == 0)
	{
		distances_[std::pair{rhstop,lhstop }] = length;
	}
}

void TransportCatalogue::SetRoutingSettings(const int bus_wait_time, const int bus_velocity) {
	bus_wait_time_ = bus_wait_time;
	bus_velocity_ = bus_velocity;
}


void TransportCatalogue::CreateGraph() {
	if (graph_ != nullptr) { delete graph_.release(); }
	size_t graph_size = stops_.size();
	graph_ = std::make_unique<Graph>(std::move(Graph(graph_size * 2)));
	for (size_t i = 0; i < graph_size; i++) {
		graph_->AddEdge({ i, graph_size + i, static_cast<double>(bus_wait_time_), true, deque_stops_.at(i).stop_name });
	}

	for (auto& bus : deque_buses_) {
		auto stops = bus.stops;

		auto addEdges = [&](auto begin, auto end) {
			for (auto it = begin; it != end; ++it) {
				auto stop_one = *it;
				double distance = 0;
				int span = 0;
				for (auto it2 = it + 1; it2 != end; ++it2) {
					auto stop_two = *it2;
					distance += GetDistance(*(it2 - 1), stop_two);
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

std::optional<Edges> TransportCatalogue::GetRouteInfo(const Stop* from, const Stop* to){
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
	return Edges{std::move(route.value().weight), std::move(edges)};
}

std::optional<Edges> TransportCatalogue::GetRouteInfo(const std::string_view from, const std::string_view to) {
	return GetRouteInfo(GetStop(from), GetStop(to));
}
