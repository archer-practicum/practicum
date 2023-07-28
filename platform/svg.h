#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include <optional>
#include <variant>

namespace svg {

using namespace std::literals;

struct Rgb {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    Rgb(size_t r, size_t g, size_t b) : red(r), green(g), blue(b) {};
    Rgb() = default;
};

struct Rgba : public Rgb {
    double opacity = 1.0;
    Rgba(size_t r, size_t g, size_t b, double op) : Rgb(r, g, b), opacity(op) {};
    Rgba() = default;
};

using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;
inline const Color NoneColor = std::monostate{};

struct OstreamColorPrinter {
    std::ostream &out;

    void operator()(std::monostate) const { out << "none"sv; }

    void operator()(const Rgb &rgb) const { 
        out << "rgb("sv 
        << std::to_string(rgb.red) << ","sv 
        << std::to_string(rgb.green) << ","sv
        << std::to_string(rgb.blue) << ")"sv; 
    }

    void operator()(const Rgba &rgba) const { 
        out << "rgba("sv 
        << std::to_string(rgba.red) << ","sv 
        << std::to_string(rgba.green) << ","sv
        << std::to_string(rgba.blue) << ","sv 
        << rgba.opacity << ")"sv; 
        }
        
    void operator()(const std::string& str) const { out << str; }    
};

inline std::ostream& operator<<(std::ostream &out, const Color& color) {
    std::visit(OstreamColorPrinter{out}, color);
    return out;
}

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

inline std::ostream& operator<<(std::ostream &out, const StrokeLineCap &line_cap) {
    switch (line_cap) {
    case StrokeLineCap::BUTT:
        out << "butt"sv;
        break;
    case StrokeLineCap::ROUND:
        out << "round"sv;
        break;
    case StrokeLineCap::SQUARE:
        out << "square"sv;
        break;
    default:
        break;
    }
    return out;
}

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<<(std::ostream &out, const StrokeLineJoin &line_join) {
    switch (line_join) {
    case StrokeLineJoin::ARCS:
        out << "arcs"sv;
        break;
    case StrokeLineJoin::BEVEL:
        out << "bevel"sv;
        break;
    case StrokeLineJoin::MITER:
        out << "miter"sv;
        break;
    case StrokeLineJoin::MITER_CLIP:
        out << "miter-clip"sv;
        break;
    case StrokeLineJoin::ROUND:
        out << "round"sv;
        break;
    default:
        break;
    }
    return out;
}

template<typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        _fill_color = color;
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        _stroke_color = color;
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        _stroke_width = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        _line_cap = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        _line_join = line_join;
        return AsOwner();
    }

protected:
    virtual ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        if (_fill_color) out << " fill=\""sv << *_fill_color << "\""sv;
        if (_stroke_color) out << " stroke=\""sv << *_stroke_color << "\""sv;
        if (_stroke_width) out << " stroke-width=\""sv << *_stroke_width << "\""sv;
        if (_line_cap) out << " stroke-linecap=\""sv << *_line_cap << "\""sv;
        if (_line_join) out << " stroke-linejoin=\""sv << *_line_join << "\""sv;
    }

//private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> _fill_color;
    std::optional<Color> _stroke_color;
    std::optional<double> _stroke_width;
    std::optional<StrokeLineCap> _line_cap;
    std::optional<StrokeLineJoin> _line_join;
};

class Object;

class ObjectContainer {
public:
    template<typename Obj>
    void Add(Obj obj) { AddPtr(std::make_unique<Obj>(std::move(obj))); }

    virtual void AddPtr(std::unique_ptr<Object> &&ptr) = 0;

    virtual ~ObjectContainer() = default;
};


class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable() = default; 
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

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;
    
    std::deque<Point> _points;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
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

private:
    void RenderObject(const RenderContext& context) const override;

    Point _pos;
    Point _offset;
    uint32_t _font_size = 1;
    std::string _font_family;
    std::string _font_weight;
    std::string _data;
};

class Document : public ObjectContainer {
public:    

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document
private:
    std::deque<std::unique_ptr<Object>> _shapes;
};

}  // namespace svg