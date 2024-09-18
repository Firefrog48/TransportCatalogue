#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {


    struct Rgb {
        Rgb() = default;
        
        Rgb(uint8_t red_, uint8_t green_, uint8_t blue_)
            : red(red_)
            , green(green_)
            , blue(blue_) {
        }

        Rgb(std::vector<double>& arr) {
            if (arr.size() != 3) {
                throw std::logic_error("");
            } 
            red = static_cast<uint8_t>(arr[0]);
            green = static_cast<uint8_t>(arr[1]);
            blue = static_cast<uint8_t>(arr[2]);
        }

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        Rgba() = default;

        Rgba(uint8_t red_, uint8_t green_, uint8_t blue_, double opacity_)
            : red(red_)
            , green(green_)
            , blue(blue_)
            , opacity(opacity_) {
        }

        Rgba(std::vector<double>& arr) {
            if (arr.size() != 4) {
                throw std::logic_error("");
            } 
            red = static_cast<uint8_t>(arr[0]);
            green = static_cast<uint8_t>(arr[1]);
            blue = static_cast<uint8_t>(arr[2]);
            opacity = arr[3];
        }

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

    // Объявив в заголовочном файле константу со спецификатором inline,
    // мы сделаем так, что она будет одной на все единицы трансляции,
    // которые подключают этот заголовок.
    // В противном случае каждая единица трансляции будет использовать свою копию этой константы
    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& cap);
    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& join);
    std::ostream& operator<<(std::ostream& out, const Color& color);

    template <typename Owner>
    class PathProps {
    public:
        //задаёт значение свойства fill — цвет заливки. По умолчанию свойство не выводится.
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        // задаёт значение свойства stroke — цвет контура. По умолчанию свойство не выводится.
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        // задаёт значение свойства stroke-width — толщину линии. По умолчанию свойство не выводится.
        Owner& SetStrokeWidth(double width) {
            width_ = width;
            return AsOwner();
        }
        // задаёт значение свойства stroke-linecap — тип формы конца линии. По умолчанию свойство не выводится.
        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }
        // задаёт значение свойства stroke-linejoin — тип формы соединения линий. По умолчанию свойство не выводится.
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();      
        }

    protected:
        ~PathProps() = default;

        // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (width_) {
                out << " stroke-width=\""sv << *width_ << "\""sv;
            }
            if (line_cap_) {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_) {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            } 
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };


    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
    * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
    * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
    */


    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
    * Абстрактный базовый класс Object служит для унифицированного хранения
    * конкретных тегов SVG-документа
    * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
    */
    class Object {
    public:

        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    /*
    * Класс Circle моделирует элемент <circle> для отображения круга
    * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
    */
    class Circle final : public Object, public PathProps<Circle> {
    public:

        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        
        void RenderObject(const RenderContext& context) const override;
        Point center_ = {0.0, 0.0};
        double radius_ = 1.0;
    };

    /*
    * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
    * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
    */
    class Polyline final : public Object, public PathProps<Polyline>{
    public:

        Polyline() = default;
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

        /*
        * Прочие методы и данные, необходимые для реализации элемента <polyline>
        */
    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;

    };

    /*
    * Класс Text моделирует элемент <text> для отображения текста
    * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
    */
    class Text final : public Object, public PathProps<Text>{
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);
        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);
        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);
        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);
        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);
        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void TransformText(std::string& data);
        void RenderObject(const RenderContext& context) const override;
        
        Point position_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t size_ = 1;
        std::string font_family_ = "";
        std::string font_weight_ = "";
        std::string data_ = "";
    };


    class ObjectContainer {

    public:
        // ObjectContainer(/* args */);
        virtual ~ObjectContainer() = default;

        /*
        Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
        Пример использования:
        Document doc;
        doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */
        template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        } 

        virtual void AddPtr(std::unique_ptr<Object>&& object) = 0;
        
    protected:
        std::vector<std::unique_ptr<Object>> objects_;
    private:
        
    };

    class Drawable {

    public:
        
        virtual void Draw(ObjectContainer& container) const  = 0;
        
        virtual ~Drawable() = default;

    private:
        
    };

    class Document : public ObjectContainer{
    public:
        

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

        // Прочие методы и данные, необходимые для реализации класса Document

    private:
        
    };

}  // namespace svg