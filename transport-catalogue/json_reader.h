#pragma once

#include "json_builder.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <variant>

namespace TransportCatalogue {

    namespace Input {

        class JsonReader {
        public:
            
            JsonReader() = default;
            JsonReader(std::istream &in);

            void LoadAndParseStatRequests(RequestHandler::RequestHandler &rh) const ;
            void LoadBaseRequests(TransportCatalogue& catalogue) const;
            void LoadRenderingSettings(MapRenderer::MapRenderer& renderer) const;
            void LoadRoutingSettings(Router::TransportRouter& router) const;

            void PrintStatRequests(RequestHandler::RequestHandler& rh, std::ostream& out);
            
        private:
            static void LoadStopsDistances(const std::unordered_map<std::string,JSON::Dict>& stops, TransportCatalogue& catalogue);
            static void LoadBuses(const std::unordered_map<std::string, std::pair<JSON::Array, bool>>& buses, TransportCatalogue& catalogue);
            static svg::Color GetColorFromJson(const JSON::Node& node);
            static JSON::Dict PackBusInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info);
            static JSON::Dict PackStopInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info);
            static JSON::Dict PackMapInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info);
            static JSON::Dict PackRouteInfoIntoJsonDict(int id, const RequestHandler::RequestInfo& info);

            JSON::Document input_;
            JSON::Document print_info_;
        };
    
    }
}

