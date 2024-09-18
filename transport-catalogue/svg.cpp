#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& cap) {
        switch(cap) {
            case StrokeLineCap::BUTT:
                out << "butt";
                break;
            case StrokeLineCap::ROUND:
                out << "round";
                break;
            case StrokeLineCap::SQUARE:
                out << "square";
                break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& join) {
        switch(join) {
            case StrokeLineJoin::ARCS:
                out << "arcs";
                break;
            case StrokeLineJoin::BEVEL:
                out << "bevel";
                break;
            case StrokeLineJoin::MITER:
                out << "miter";
                break;
            case StrokeLineJoin::MITER_CLIP:
                out << "miter-clip";
                break;
            case StrokeLineJoin::ROUND:
                out << "round";
                break;
        }
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const Color &color) {
        
        if (std::holds_alternative<std::monostate>(color)) {
            out << svg::NoneColor;
        
        } else if(std::holds_alternative<std::string>(color)) {
            out << std::get<std::string>(color);
        
        } else if(std::holds_alternative<Rgb>(color)) {
            Rgb rgb = std::get<svg::Rgb>(color);
            out << "rgb("sv << static_cast<int>(rgb.red) 
                << ',' << static_cast<int>(rgb.green) 
                << ',' << static_cast<int>(rgb.blue) << ')';
        
        } else if(std::holds_alternative<Rgba>(color)) { 
            Rgba rgba = std::get<svg::Rgba>(color);
            out << "rgba("sv << static_cast<int>(rgba.red) 
                << ',' << static_cast<int>(rgba.green) 
                << ',' << static_cast<int>(rgba.blue) 
                << ',' << rgba.opacity << ')';
        }
        return out;
    }

    // ============Object==================================
    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------


    Circle &Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }



    // ==============================polyline===================================================

    Polyline &Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        bool is_first = true;
        auto& out = context.out;
        out << "<polyline points=\""sv;
        
        for (const auto point : points_) {
            if (is_first) {
                out << point.x << ',' << point.y;
                is_first = false;
            } else {
                out << ' ' << point.x << ',' << point.y;
            } 
        }
        
        out << "\"";
        RenderAttrs(out);
        out << "/>"sv;

    }
    // ==============================text===================================================
    Text &Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    Text &Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text &Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    Text &Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text &Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text &Text::SetData(std::string data) {
        TransformText(data);
        return *this;
    }

    void Text::TransformText(std::string& data) {
        for (char ch : data) {
            switch (ch) {
                case '"':
                    data_ += "&quot;";
                    break;
                case ';':
                    data_ += "&semi;";
                    break;
                case '\'':
                    data_ += "&apos;";
                    break;
                case '<':
                    data_ += "&lt;";
                    break;
                case '>':
                    data_ += "&gt;";
                    break;
                case '&':
                    data_ += "&amp;";
                    break;
                default:
                    data_ += ch;
            }
        }
    }

    void Text::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << "<text";
        RenderAttrs(out);
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;  
        out << "font-size=\""sv << size_ << "\""sv;
        
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">"sv << data_ << "</text>"sv;
    }
    // ==============================document===================================================
    void Document::Render(std::ostream &out) const {        
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        RenderContext context(out, 2, 2);
        for (const auto& obj : objects_) {
            obj->Render(context);
        }
        out << "</svg>";
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj)  {
        objects_.emplace_back(std::move(obj));
    }

} // namespace svg