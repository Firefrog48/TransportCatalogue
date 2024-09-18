#include <iostream>
#include <string>

#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <cassert>

#include <fstream>
#include <sstream>

using namespace std;


int main() {
     
    TransportCatalogue::TransportCatalogue catalogue;
    TransportCatalogue::Input::JsonReader reader(cin);
    MapRenderer::MapRenderer renderer;
    TransportCatalogue::Router::TransportRouter router(catalogue);
    RequestHandler::RequestHandler request_handler(catalogue, renderer, router);
    reader.LoadBaseRequests(catalogue);
    reader.LoadRenderingSettings(renderer);
    reader.LoadRoutingSettings(router);
    reader.LoadAndParseStatRequests(request_handler);
    reader.PrintStatRequests(request_handler, cout);
    
}
