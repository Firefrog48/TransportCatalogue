#pragma once

#include "domain.h"
#include "geo.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace TransportCatalogue {
	

	class TransportCatalogue {
		

	public:
		void AddStop(const std::string_view stop_name, const Geo::Coordinates& coordinates);
		void SetDistanceBetweenStops(std::string_view stop1_name, std::string_view stop2_name, double length);
		void AddBus(const std::string_view bus_name,const std::vector<std::string_view>& stops, bool is_roundtrip_);

		const std::unordered_map<std::string_view, Bus*>& GetReferenseBuses() const;
		const std::unordered_map<std::string_view, Stop*>& GetReferenseStops() const;
		
		double ComputeRouteLength(const Bus* bus) const;
		Info::Bus GetInfoAboutBus (std::string_view bus) const;
		Info::Stop GetInfoAboutStop (std::string_view stop) const;
		double GetDistanceBetweenStops(Stop* from, Stop* to) const;

	private:

		class StopsPtrHasher {
		public:
		size_t operator()(const std::pair<Stop*,Stop*> stops_ptr_pair) const {
			std::hash<const void*> hasher;
			const int q = 37;
			size_t hash = (hasher(stops_ptr_pair.first) / q ) + (hasher(stops_ptr_pair.second) * q);
			return hash;
		}
		};

		//имя остановки -> Stop_ptr
		std::unordered_map<std::string_view, Stop*> stop_name_to_stops_;
		//остановки
		std::deque<Stop> stops_;
		//имя автобуса -> Bus_ptr
		std::unordered_map<std::string_view, Bus*> bus_name_to_bus_;
		//автобусы
		std::deque<Bus> buses_;
		//расстояния между остановками
		std::unordered_map<std::pair<Stop*, Stop*>, int, StopsPtrHasher> length_between_stops_;
		
	};
}



