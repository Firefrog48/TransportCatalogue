#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "router.h"
#include "transport_catalogue.h"


namespace TransportCatalogue{
    namespace Router{
        using Minutes = double;
        const double MINUTES_IN_HOUR = 60.;
        const double METRS_IN_KILOMETR = 1000.;

        class TransportRouter {
        public:
            TransportRouter(TransportCatalogue& catalogue);
            void SetSettings(Info::Router::RoutingSettings& settings);
            Info::Route GetRouteInfo(std::pair<std::string_view, std::string_view> pair_stop_from_to) const;
        private:
            void AddEdges();
            void AddVertexes();
            void SetGraph();
            
            Minutes CalculateTimeBetweenStops(Stop* stop_ptr_from, Stop* stop_ptr_to) const {
                return  catalogue_.GetDistanceBetweenStops(stop_ptr_from, stop_ptr_to) * MINUTES_IN_HOUR / (settings_.bus_velocity * METRS_IN_KILOMETR);
            }

            template <typename It>
            void ConnectBusStops(It begin, It end, std::string_view bus_name) {
                for(auto from_it = begin; from_it != std::prev(end); from_it++) {
                    Minutes weight = 0.;
                    int span_count = 0;
                    
                    for(auto to_it = std::next(from_it); to_it != end; to_it++) {
                        std::string_view from_stop = (*from_it)->name;
                        auto vertex_from_stop = vertexes_.at(from_stop).hub;

                        std::string_view to_stop = (*to_it)->name;
                        auto vertex_to_stop = vertexes_.at(to_stop).portal;

                        weight += CalculateTimeBetweenStops(*std::prev(to_it), *to_it);
                        span_count++;

                        auto bus_edge_id = graph_.AddEdge({vertex_from_stop, vertex_to_stop, weight});
                        id_to_bus_route_info[bus_edge_id] = {bus_name, span_count, weight};
                    }  
                }
            }

            graph::DirectedWeightedGraph<Minutes> graph_;
            std::unique_ptr<graph::Router<Minutes>> router_ = nullptr;
            Info::Router::RoutingSettings settings_;
            TransportCatalogue& catalogue_;
            graph::VertexId counter_ = 0;
            std::unordered_map<std::string_view, graph::VertexIds> vertexes_;
            std::unordered_map<graph::EdgeId, Info::Router::WaitInfo> id_to_wait_info_;
            std::unordered_map<graph::EdgeId, Info::Router::BusRouteInfo> id_to_bus_route_info;

        };

    }
}