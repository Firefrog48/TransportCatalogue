#include "request_handler.h"

using namespace std::literals;

RequestHandler::RequestHandler::RequestHandler(TransportCatalogue::TransportCatalogue &catalogue, MapRenderer::MapRenderer &renderer, TransportCatalogue::Router::TransportRouter &router)  
    : data_base_(&catalogue)
    , map_renderer_(&renderer)
    , router_(&router) {
}


void RequestHandler::RequestHandler::AddStatRequest(int id, std::string&& type, RequestValue&& name) {
    stat_requests_.emplace_back(std::move(StatRequest(id, type, name)));
}

std::vector<std::pair<int, RequestHandler::RequestInfo>> RequestHandler::RequestHandler::GetRequestInfo() const noexcept {
    return requests_info_;
}

void RequestHandler::RequestHandler::ParseStats() {
    UploadRendererMap();
    for (auto&& request : stat_requests_) {
        if (request.type == "Bus"s) {
            requests_info_.emplace_back(request.id, std::move(data_base_->GetInfoAboutBus(std::get<std::string>(request.value))));
        } else if (request.type == "Stop"s) {
            requests_info_.emplace_back(request.id, std::move(data_base_->GetInfoAboutStop(std::get<std::string>(request.value))));
        } else if (request.type == "Route"s) {
            requests_info_.emplace_back(request.id, std::move(router_->GetRouteInfo(std::get<std::pair<std::string, std::string>>(request.value))));
        } else if (request.type == "Map"s) {
            std::ostringstream ss;
            map_renderer_->RenderMap(ss);
            requests_info_.emplace_back(request.id, std::move(ss.str()));
        }
    }
}


void RequestHandler::RequestHandler::UploadRendererMap() {
    auto buses = data_base_->GetReferenseBuses();
    auto stops = data_base_->GetReferenseStops();
    map_renderer_->UpdateRendererMap(buses, stops);

}

