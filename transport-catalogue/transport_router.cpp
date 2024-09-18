#include "transport_router.h"

// Вставьте сюда решение из предыдущего спринта

namespace TransportCatalogue{
    namespace Router{
        
        TransportRouter::TransportRouter(TransportCatalogue &catalogue) : catalogue_(catalogue) {
        }
        void TransportRouter::SetSettings(Info::Router::RoutingSettings &settings) {
            settings_ = std::move(settings);
            SetGraph();
            router_ = std::make_unique<graph::Router<Minutes>>(graph_);
        }

        Info::Route TransportRouter::GetRouteInfo(std::pair<std::string_view, std::string_view> pair_stop_from_to) const {
            Info::Route result;
            auto stop_from = vertexes_.at(pair_stop_from_to.first).portal;
            auto stop_to = vertexes_.at(pair_stop_from_to.second).portal;

            auto route_info = router_->BuildRoute(stop_from, stop_to);

            if(route_info) {
                result.not_found = false;
                result.total_time = route_info->weight;
                for (const auto& edge_id : route_info->edges) {
                    if(id_to_wait_info_.count(edge_id)) {
                        result.items_.emplace_back(id_to_wait_info_.at(edge_id));
                    } else if (id_to_bus_route_info.count(edge_id)) {
                        result.items_.emplace_back(id_to_bus_route_info.at(edge_id));
                    }
                }
            }
            return result;
        }

        void TransportRouter::AddEdges() {
            // добавляем ребра внутри одной остановки между посадкой и высадкой
            for (const auto& [stop_name, stop_ptr] : catalogue_.GetReferenseStops()) {
                auto edge_id = graph_.AddEdge({vertexes_.at(stop_name).portal, vertexes_.at(stop_name).hub, settings_.bus_wait_time});
                id_to_wait_info_[edge_id] = {stop_name, settings_.bus_wait_time};
            }
            // добавляем рёбра между остановками автобуса
            for (const auto& [bus_name, bus_ptr] : catalogue_.GetReferenseBuses()) {
                ConnectBusStops(bus_ptr->stops_of_the_bus.begin(), bus_ptr->stops_of_the_bus.end(), bus_name);
                if (!bus_ptr->is_roundtrip) {
                    ConnectBusStops(bus_ptr->stops_of_the_bus.rbegin(), bus_ptr->stops_of_the_bus.rend(), bus_name);
                }
            }
        }

        void TransportRouter::SetGraph() {
            AddVertexes();
            graph_ = graph::DirectedWeightedGraph<Minutes>(counter_);
            AddEdges();
        }

        void TransportRouter::AddVertexes() {
            for (const auto& [stop_name, stop_ptr] : catalogue_.GetReferenseStops()) {
                vertexes_[stop_name].portal = counter_++; 
                vertexes_[stop_name].hub = counter_++;
            }
        }
        
    }
}


