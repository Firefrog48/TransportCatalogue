#include "transport_catalogue.h"


namespace TransportCatalogue {

	void TransportCatalogue::AddStop(const std::string_view stop_name, const Geo::Coordinates& stop_coordinates) {
		stops_.emplace_back(std::string(stop_name), stop_coordinates);
		Stop* ptr = &stops_.back();
		stop_name_to_stops_[ptr->name] = ptr;
	}

	void TransportCatalogue::TransportCatalogue::SetDistanceBetweenStops(std::string_view stop1_name, std::string_view stop2_name, double length) {
		Stop* ptr1 = stop_name_to_stops_.at(stop1_name);
		Stop* ptr2 = stop_name_to_stops_.at(stop2_name);
		length_between_stops_[{ptr1, ptr2}] = length;
	}

	void TransportCatalogue::AddBus(const std::string_view bus_name,const std::vector<std::string_view>& stops, bool is_roundtrip_) {
		buses_.emplace_back(Bus(std::string(bus_name),is_roundtrip_));
		Bus* bus_ptr = &buses_.back();
		bus_name_to_bus_[bus_ptr->name] = bus_ptr;
		
		for(auto&& stop : stops) {
			Stop* stop_ptr = stop_name_to_stops_.at(stop);
			bus_ptr->stops_of_the_bus.push_back(stop_ptr);
			stop_ptr->buses_of_the_stop.insert(bus_ptr);
		}
	}


	Info::Bus TransportCatalogue::GetInfoAboutBus(std::string_view bus) const{
		Info::Bus businfo(bus);
		auto pos = bus_name_to_bus_.find(bus);
		
		if (pos == bus_name_to_bus_.end()) {
			return businfo;
		
		} else {
			businfo.no_bus = false;
			businfo.count_of_stops = pos->second->stops_of_the_bus.size();
			std::set<Stop*> unique_stop(pos->second->stops_of_the_bus.begin(), pos->second->stops_of_the_bus.end());
			businfo.count_of_unique_stops = unique_stop.size();
			double geo_length = Geo::detail::ComputeGeoLength(pos->second->stops_of_the_bus);
			businfo.length = ComputeRouteLength(pos->second);
			businfo.curvature = businfo.length / geo_length;
			return businfo;
		}
	}

	Info::Stop TransportCatalogue::GetInfoAboutStop(std::string_view stop_name) const{
		Info::Stop stop_info(stop_name);
		auto pos = stop_name_to_stops_.find(stop_name);
		
		if (pos == stop_name_to_stops_.end()) {
			return stop_info;
		}
		stop_info.not_found = false;
		
		for (auto&& bus : pos->second->buses_of_the_stop){
			stop_info.buses_on_stop.push_back(bus->name);
			std::sort(stop_info.buses_on_stop.begin(), stop_info.buses_on_stop.end());
		}	
		return stop_info;
	}

    double TransportCatalogue::GetDistanceBetweenStops(Stop* from, Stop* to) const {
		double result = 0.;
		auto it = length_between_stops_.find({from, to});
        if (it != length_between_stops_.end()) {
			result = length_between_stops_.at({from, to});
		} else if (length_between_stops_.find({to, from}) != length_between_stops_.end()) {
			result = length_between_stops_.at({to, from});
		}
		return result;
    }

    const std::unordered_map<std::string_view, Bus *>& TransportCatalogue::GetReferenseBuses() const {
        return bus_name_to_bus_;
    }

    const std::unordered_map<std::string_view, Stop *>& TransportCatalogue::GetReferenseStops() const {
        return stop_name_to_stops_;
    }

    double TransportCatalogue::ComputeRouteLength(const Bus* bus) const {		
		double result = 0;
		size_t end_of_calculating = static_cast<int>(bus->stops_of_the_bus.size()) - 1;
			for(size_t i = 0; i < end_of_calculating;) {
				Stop* stop1 = bus->stops_of_the_bus[i];
				Stop* stop2 = bus->stops_of_the_bus[++i];
				result += GetDistanceBetweenStops(stop1, stop2);
			}
		return result;
	}

    
}




