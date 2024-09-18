#include "json_reader.h"

namespace TransportCatalogue {
    namespace Input {
        using namespace std::literals;


        JsonReader::JsonReader(std::istream &in) : input_(JSON::Load(in)) {
        }


        void JsonReader::LoadAndParseStatRequests(RequestHandler::RequestHandler& rh) const {
            auto requests = input_.GetRoot().AsDict();
            auto stat_requests_ = requests.at("stat_requests"s).AsArray();
            
            for (auto&& stat_request : stat_requests_) {
                auto request = stat_request.AsDict();
                auto request_id = request.at("id"s).AsInt();
                auto request_type = request.at("type"s).AsString();
                
                if(request_type == "Bus"s || request_type == "Stop"s) {
                    auto request_name = request.at("name"s).AsString();
                    rh.AddStatRequest(request_id, std::move(request_type), std::move(request_name));
                } else if (request_type == "Route"s) {
                    std::pair<std::string, std::string> stop_from_to = {request.at("from"s).AsString(), request.at("to"s).AsString()};
                    rh.AddStatRequest(request_id, std::move(request_type), stop_from_to);
                } else if (request_type == "Map"s) {
                    rh.AddStatRequest(request_id, std::move(request_type), ""s);
                }
            }
            rh.ParseStats();   
        }
        

        void JsonReader::LoadBaseRequests(TransportCatalogue &catalogue) const {
            std::unordered_map<std::string, JSON::Dict> length_between_stops;
            std::unordered_map<std::string, std::pair<JSON::Array, bool>> buses;
            auto requests = input_.GetRoot().AsDict();
            auto base_requests_ = requests.at("base_requests"s).AsArray();
            
            for (auto&& base_request : base_requests_) {
                auto request = base_request.AsDict();
                std::string request_type = request.at("type"s).AsString();
                
                if (request_type == "Stop"s) {
                    auto stop_name = request.at("name"s).AsString();
                    Geo::Coordinates coordinates = {request.at("latitude"s).AsDouble(), request.at("longitude"s).AsDouble()};
                    catalogue.AddStop(stop_name, coordinates);
                    length_between_stops[stop_name] = request.at("road_distances"s).AsDict();
                
                } else if (request_type == "Bus"s) {
                    bool is_roundtrip = request.at("is_roundtrip").AsBool();
                    std::string bus_name = request.at("name").AsString();
            
                    if (is_roundtrip) {
                        buses[bus_name] = {request.at("stops"s).AsArray(), is_roundtrip};
                    
                    } else if (!is_roundtrip) {
                        auto stops = request.at("stops"s).AsArray();
                        stops.reserve(stops.size() * 2);
                        stops.insert(stops.end(), ++stops.rbegin(), stops.rend());
                        buses[bus_name] = {std::move(stops), is_roundtrip};
                    } 
                }
            }
            LoadStopsDistances(std::move(length_between_stops), catalogue);
            LoadBuses(std::move(buses), catalogue);
        }


        void JsonReader::LoadRenderingSettings(MapRenderer::MapRenderer& renderer) const {
            MapRenderer::RenderSettings render_settings;

            auto requests = input_.GetRoot().AsDict();
            auto render_settings_ = requests.at("render_settings"s).AsDict();
            render_settings.bus_label_font_size = render_settings_.at("bus_label_font_size"s).AsDouble();
            for (const auto& elem : render_settings_.at("bus_label_offset"s).AsArray()) {
                render_settings.bus_label_offset.emplace_back(elem.AsDouble());
            }
            for (const auto& elem : render_settings_.at("color_palette"s).AsArray()){
                render_settings.color_palette.emplace_back(GetColorFromJson(elem));
            }     
            render_settings.height = render_settings_.at("height"s).AsDouble();
            render_settings.line_width = render_settings_.at("line_width"s).AsDouble();
            render_settings.underlayer_width = render_settings_.at("underlayer_width"s).AsDouble();
            render_settings.padding = render_settings_.at("padding"s).AsDouble();
            render_settings.stop_label_font_size = render_settings_.at("stop_label_font_size"s).AsDouble();
            for (const auto& elem : render_settings_.at("stop_label_offset"s).AsArray()){
                render_settings.stop_label_offset.emplace_back(elem.AsDouble());
            }
            render_settings.stop_radius = render_settings_.at("stop_radius"s).AsDouble();
            render_settings.underlayer_color = GetColorFromJson(render_settings_.at("underlayer_color"s));
            render_settings.width = render_settings_.at("width"s).AsDouble();

            renderer.SetRenderSettings(std::move(render_settings));
        }
        void JsonReader::LoadRoutingSettings(Router::TransportRouter &router) const {
            auto requests = input_.GetRoot().AsDict();
            auto routing_settings_dict = requests.at("routing_settings"s).AsDict();
            Info::Router::RoutingSettings result;
            result.bus_velocity = routing_settings_dict.at("bus_velocity"s).AsDouble();
            result.bus_wait_time = routing_settings_dict.at("bus_wait_time"s).AsDouble();
            router.SetSettings(result);
        }

        void JsonReader::PrintStatRequests(RequestHandler::RequestHandler& rh, std::ostream& out) {
            JSON::Builder result;
            result.StartArray();
            for (const auto& [id, info] : rh.GetRequestInfo()) {
                
                if (std::holds_alternative<Info::Bus>(info)) {
                    result.Value(PackBusInfoIntoJsonDict(id, info));
                } else if (std::holds_alternative<Info::Stop>(info)) {
                    result.Value(PackStopInfoIntoJsonDict(id, info));
                } else if (std::holds_alternative<Info::Route>(info)) {
                    result.Value(PackRouteInfoIntoJsonDict(id, info));
                } else if (std::holds_alternative<std::string>(info)) {
                    result.Value(PackMapInfoIntoJsonDict(id,info));
                }
            }
            JSON::Print(JSON::Document{result.EndArray().Build()}, out);
        }

    }


    void Input::JsonReader::LoadStopsDistances(const std::unordered_map<std::string,JSON::Dict>& stops, TransportCatalogue& catalogue) {
        for (const auto& [stop1_name, length_to_stops] : stops) {
            for (const auto& [stop2_name, length] : length_to_stops) {
                catalogue.SetDistanceBetweenStops(stop1_name, stop2_name, length.AsDouble());
            }
        }
    }


    void Input::JsonReader::LoadBuses(const std::unordered_map<std::string, std::pair<JSON::Array, bool>>& buses, TransportCatalogue& catalogue) {
        for (const auto& [bus_name, stops_and_bool] : buses) {
            const auto& [stops, is_roundtrip] = stops_and_bool;
            std::vector<std::string_view> stops_of_the_bus_view;
            stops_of_the_bus_view.reserve(stops.size());
            
            for(const auto& stop : stops) {
                stops_of_the_bus_view.emplace_back(stop.AsString());
            }
            catalogue.AddBus(bus_name, std::move(stops_of_the_bus_view), is_roundtrip);
        }
    }


    svg::Color Input::JsonReader::GetColorFromJson(const JSON::Node& node) {
        if (node.IsArray()) {
            std::vector<double> vec;
            auto arr = node.AsArray();
            for (auto&& elem : arr) {
                vec.emplace_back(elem.AsDouble());
            }
            if (vec.size() == 3) {
                return svg::Rgb(vec);
            } else if (vec.size() == 4) {
                return svg::Rgba(vec);
            } else {
                throw std::invalid_argument("Color isn't correct"s);
            }
        } 
        return svg::Color(node.AsString());
    }


    JSON::Dict Input::JsonReader::PackBusInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info) {
        JSON::Builder result;
        result.StartDict().Key("request_id"s).Value(id);
        auto bus_info = std::get<Info::Bus>(info);
        if (bus_info.no_bus) {
            result.Key("error_message"s).Value("not found"s);
        } else {
            result.Key("curvature"s).Value(bus_info.curvature)
                  .Key("route_length"s).Value(bus_info.length)
                  .Key("stop_count"s).Value(bus_info.count_of_stops)
                  .Key("unique_stop_count"s).Value(bus_info.count_of_unique_stops);
        }
        return result.EndDict().Build().AsDict();
    }


    JSON::Dict Input::JsonReader::PackStopInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info) {
        JSON::Builder result;
        result.StartDict().Key("request_id"s).Value(id);
        auto stop_info = std::get<Info::Stop>(info);          
        if (stop_info.not_found) {
            result.Key("error_message"s).Value("not found"s);
        } else {
            result.Key("buses"s).StartArray();
            for (auto&& stop : stop_info.buses_on_stop) {
                result.Value(std::string(stop));
            }
            result.EndArray();
        }
        return result.EndDict().Build().AsDict();
    }


    JSON::Dict Input::JsonReader::PackMapInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info) {
        JSON::Builder result;
        result.StartDict().Key("request_id").Value(id);
        auto svg_string = std::get<std::string>(info);
        result.Key("map"s).Value(std::move(svg_string))
              .EndDict();
        return result.Build().AsDict();
    }

    JSON::Dict Input::JsonReader::PackRouteInfoIntoJsonDict(int id, const RequestHandler::RequestInfo &info) {
        JSON::Builder result;
        result.StartDict().Key("request_id").Value(id);
        auto route_info = std::get<Info::Route>(info);
        if(route_info.not_found) {
            result.Key("error_message"s).Value("not found"s);
        } else {
            result.Key("total_time"s).Value(route_info.total_time)
                  .Key("items"s).StartArray();
            for (const auto& item : route_info.items_) {
                result.StartDict();
                if(std::holds_alternative<Info::Router::BusRouteInfo>(item)) {
                    auto bus_route_info = std::get<Info::Router::BusRouteInfo>(item);
                    result.Key("type"s).Value("Bus"s)
                          .Key("bus"s).Value(std::string(bus_route_info.bus_name))
                          .Key("span_count"s).Value(bus_route_info.span_count)
                          .Key("time"s).Value(bus_route_info.time);
                } else {
                    auto wait_info = std::get<Info::Router::WaitInfo>(item);
                    result.Key("type"s).Value("Wait"s)
                          .Key("stop_name"s).Value(std::string(wait_info.stop_name))
                          .Key("time"s).Value(wait_info.time);
                }
                result.EndDict();
            }
            result.EndArray();
        }
        return result.EndDict().Build().AsDict();
    }
}



