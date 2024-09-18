#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

#include <sstream>
#include <variant>
#include <vector>
#include <unordered_map>

namespace RequestHandler {
    using RequestInfo = std::variant<std::string, TransportCatalogue::Info::Bus, TransportCatalogue::Info::Stop, TransportCatalogue::Info::Route>; 
    using RequestValue = std::variant<std::monostate, std::string, std::pair<std::string, std::string>>;

    class RequestHandler {
    public:

        RequestHandler() = default;
        RequestHandler(TransportCatalogue::TransportCatalogue& catalogue, MapRenderer::MapRenderer& renderer, TransportCatalogue::Router::TransportRouter& router);
        
        void AddStatRequest(int id, std::string&& type, RequestValue&& name);
        std::vector<std::pair<int, RequestInfo>> GetRequestInfo() const noexcept;
        
        void ParseStats();
        
    private:
        void UploadRendererMap();
 
        struct StatRequest{
            StatRequest(int id,const std::string& type, const RequestValue& value) : id(id)
                                                                                   , type(type)
                                                                                   , value(value){                                           
            }
        public:
            int id = 0;
            std::string type;
            RequestValue value;
        };

        std::vector<StatRequest> stat_requests_;
        std::vector<std::pair<int, RequestInfo>> requests_info_;
        
        TransportCatalogue::TransportCatalogue* data_base_ = nullptr;
        MapRenderer::MapRenderer* map_renderer_ = nullptr;
        TransportCatalogue::Router::TransportRouter* router_ = nullptr;

    };
 
}

