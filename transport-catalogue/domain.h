#pragma once

#include "geo.h"

#include <deque>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

namespace TransportCatalogue {
	namespace Info{

		struct Bus {
			Bus() = default;
			Bus(std::string_view bus_name) : name(std::string(bus_name)){}
			
			public:
				std::string name;
				bool no_bus = true;
				int count_of_stops = 0;
				int count_of_unique_stops = 0;
				double length = 0.0;
				double curvature = 0;
				bool is_roundtrip = false;
			};
			
			struct Stop {
				Stop() = default;
				Stop(std::string_view stop_name) : name(std::string(stop_name)){}
			
			public:
				bool not_found = true;
				std::string name;
				std::vector<std::string_view> buses_on_stop;
			};

			namespace Router{
				struct WaitInfo{
					std::string_view stop_name;
					double time = 0.;
				};

				struct BusRouteInfo{
					std::string_view bus_name;
        			int span_count = 0;
        			double time = 0.;
				};

				struct RoutingSettings {
					double bus_velocity = 0.;
					double bus_wait_time = 0.;
				};
			}

			struct Route {
				bool not_found = true;
				double total_time = 0.;
				std::vector<std::variant<std::monostate, Router::BusRouteInfo, Router::WaitInfo>> items_;
			};
		}
	
		struct Bus;
		
		struct Stop{
		
			Stop() = default;
		
			explicit Stop(const std::string& name_stop,const Geo::Coordinates& stop_coordinates) : 
																				name(name_stop),
																				coordinates(stop_coordinates) {}
			Stop(const std::string& name_stop) : name(name_stop) {}
		
			void ChangeCoordinates(const Geo::Coordinates& stop_coordinates) {
				coordinates = stop_coordinates;
			}
		
			bool operator==(Stop& other) {
				return this->name == other.name;
			}
			
		public:
			std::string name;
			Geo::Coordinates coordinates = {0,0};
			//автобусы остановки
			std::unordered_set<Bus*> buses_of_the_stop;
		};
		
		
		
		struct Bus{
		
			Bus() = default;
			
			Bus(const std::string& bus_name, bool is_roundtrip_) : name(bus_name)
																 , is_roundtrip(is_roundtrip_) {
			}
		
			bool operator==(const Bus& other) {
				return this->name == other.name;
			}
			
		
		public:
			std::string name;
			//указатели на остановки автобуса
			std::deque<Stop*> stops_of_the_bus;
			bool is_roundtrip = false;
		};
}


