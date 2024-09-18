#include "map_renderer.h"

namespace MapRenderer {
    using namespace std::literals;
    
    void MapRenderer::UpdateRendererMap(const std::unordered_map<std::string_view, TransportCatalogue::Bus *>& buses, 
                                        const std::unordered_map<std::string_view, TransportCatalogue::Stop *>& stops) {
        {
            auto pos_begin = buses.begin();
            auto pos_end = buses.end();
            buses_.reserve(buses.size());
            for (;pos_begin != pos_end;++pos_begin) {
                buses_.emplace_back(pos_begin->second);
            }
            std::sort(buses_.begin(), buses_.end(), [](TransportCatalogue::Bus* lhs, TransportCatalogue::Bus* rhs) {
                return lhs->name < rhs->name;
            });
        }
        {
            auto pos_begin = stops.begin();
            auto pos_end = stops.end();
            stops_.reserve(stops_.size());
            for (;pos_begin != pos_end;++pos_begin) {
                stops_.emplace_back(pos_begin->second);
            }
            std::sort(stops_.begin(), stops_.end(), [](TransportCatalogue::Stop* lhs, TransportCatalogue::Stop* rhs) {
                return lhs->name < rhs->name;
            });
        }

    }

    void MapRenderer::SetBusLineSettings(svg::Polyline& line) const {
        line.SetFillColor("none"s)
            .SetStrokeColor(render_settings_.color_palette[render_settings_.number_of_current_color])
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND) 
            .SetStrokeWidth(render_settings_.line_width);
    }

    void MapRenderer::SetBusNameTextSettings(svg::Text &text, TransportCatalogue::Bus* bus_ptr) const {        
        svg::Point label_ofset = {render_settings_.bus_label_offset[0], render_settings_.bus_label_offset[1]};
        text.SetOffset(label_ofset)
            .SetFontSize(render_settings_.bus_label_font_size)
            .SetFontFamily("Verdana"s)
            .SetFontWeight("bold"s)
            .SetData(bus_ptr->name);       
    }

    void MapRenderer::SetStopNameTextSettings(svg::Text &text, TransportCatalogue::Stop* stop_ptr) const {
        svg::Point stop_label_ofset = {render_settings_.stop_label_offset[0], render_settings_.stop_label_offset[1]};
        text.SetOffset(stop_label_ofset)
            .SetFontSize(render_settings_.stop_label_font_size)
            .SetFontFamily("Verdana"s)
            .SetData(stop_ptr->name);
    }

    SphereProjector MapRenderer::MakeSpereProjector() const {
        std::vector<Geo::Coordinates> coordinates;
        coordinates.reserve(stops_.size());
        
        for (const auto& stop_ptr : stops_){
            if(!stop_ptr->buses_of_the_stop.empty()) {
                coordinates.emplace_back(stop_ptr->coordinates);
            }
        }
        return  SphereProjector(coordinates.begin(), coordinates.end(),
                            render_settings_.width, render_settings_.height, render_settings_.padding);
    }

    
    void MapRenderer::SetRightColor() noexcept {
        if (render_settings_.number_of_current_color == render_settings_.color_palette.size()) {
            render_settings_.number_of_current_color = 0;
        }
    }
    

    void MapRenderer::DrawBusLine(const SphereProjector& sphere_projector) {
        render_settings_.number_of_current_color = 0;
        
        for (const auto& bus_ptr : buses_) {    
            SetRightColor();
            svg::Polyline line;
            
            for (const auto& stop : bus_ptr->stops_of_the_bus){
                line.AddPoint(sphere_projector(stop->coordinates));
            }
            
            if (!bus_ptr->stops_of_the_bus.empty()){
                SetBusLineSettings(line);
            }
            document_.Add(line);
            render_settings_.number_of_current_color++;
        }
    }



    void MapRenderer::DrawBusName(const SphereProjector &sphere_projector) {
        render_settings_.number_of_current_color = 0;
        for (const auto& bus_ptr : buses_) { 
            SetRightColor();
            svg::Text underlayer;
            svg::Text bus_name_text;
            auto stops = bus_ptr->stops_of_the_bus;
            
            underlayer.SetFillColor(render_settings_.underlayer_color)
                    .SetStrokeColor(render_settings_.underlayer_color)
                    .SetStrokeWidth(render_settings_.underlayer_width)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            bus_name_text.SetFillColor(render_settings_.color_palette[render_settings_.number_of_current_color]);
            
            SetBusNameTextSettings(underlayer, bus_ptr);
            SetBusNameTextSettings(bus_name_text, bus_ptr);

            auto first_stop = stops.front();
            auto point = sphere_projector(first_stop->coordinates);
            underlayer.SetPosition(point);
            bus_name_text.SetPosition(point);
            document_.Add(underlayer);
            document_.Add(bus_name_text);
                
            if (!bus_ptr->is_roundtrip) {
                int pos_of_last_stop = stops.size() / 2;
                auto last_stop = stops[pos_of_last_stop];
                
                if (last_stop != first_stop) {
                    auto point = sphere_projector(last_stop->coordinates);
                    underlayer.SetPosition(point);
                    bus_name_text.SetPosition(point);
                    document_.Add(underlayer);
                    document_.Add(bus_name_text);  
                }      
            }
            render_settings_.number_of_current_color++;
        }
        
    }

    void MapRenderer::DrawStopCircles(const SphereProjector &sphere_projector) {
        for (const auto& stop : stops_) { 
            
            if(!stop->buses_of_the_stop.empty()){       
                svg::Circle stop_circle;
                stop_circle.SetCenter(sphere_projector(stop->coordinates));
                stop_circle.SetRadius(render_settings_.stop_radius);
                stop_circle.SetFillColor("white"s);
                document_.Add(stop_circle);
            }
        }    
        
    }

    void MapRenderer::DrawStopNames(const SphereProjector &sphere_projector) {
        for (const auto& stop : stops_) { 
            if(!stop->buses_of_the_stop.empty()){       
                svg::Text stop_name_text;
                svg::Text underlayer;
                svg::Point point = sphere_projector(stop->coordinates);
                
                stop_name_text.SetPosition(point)
                              .SetFillColor("black"s);
                
                underlayer.SetPosition(point)
                          .SetFillColor(render_settings_.underlayer_color)
                          .SetStrokeColor(render_settings_.underlayer_color)
                          .SetStrokeWidth(render_settings_.underlayer_width)
                          .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                          .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                SetStopNameTextSettings(stop_name_text, stop);
                SetStopNameTextSettings(underlayer, stop);
                
                document_.Add(underlayer);
                document_.Add(stop_name_text);

            }
        }
    }

    void MapRenderer::RenderMap(std::ostream &out) {
        bool is_empty = render_settings_.color_palette.empty() || buses_.empty() || stops_.empty();
        if(!is_empty){
            SphereProjector sphere_projector = MakeSpereProjector();
            DrawBusLine(sphere_projector);
            DrawBusName(sphere_projector);
            DrawStopCircles(sphere_projector);
            DrawStopNames(sphere_projector);
        }
        
        document_.Render(out);
    }

    void MapRenderer::SetRenderSettings(RenderSettings&& render_settings) {
        render_settings_ = std::move(render_settings);
    }

    bool SphereProjector::IsZero(double value){
            return std::abs(value) < EPSILON;
    }
}
