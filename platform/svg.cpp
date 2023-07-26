#define _USE_MATH_DEFINES 
#include <cmath>
#include "svg.h"
#include "sstream"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ----------Document------------------

void Document::AddPtr(std::unique_ptr<Object>&& obj) { _shapes.push_back(std::move(obj)); }

void Document::Render(std::ostream& out) const {
    RenderContext context(out, 2, 2);

    out <<  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

    for (const auto &shape : _shapes) shape->Render(context);
    
    out << "</svg>"sv << std::endl;
    
    }

// ------------Polyline-----------------

Polyline& Polyline::AddPoint(Point point) {
    _points.emplace_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto &out = context.out;
    out << "<polyline points="sv;
    out << "\""sv;
    bool first_el = true;
    for (const auto &point : _points) {
        if (!first_el) out << " "sv;
        else first_el = false;
        out << point.x << ","sv << point.y; 
    }
    out << "\""sv;
    out << " />"sv;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ----------------------Text-------------------------

Text& Text::SetPosition(Point pos) {
    _pos = std::move(pos);
    return *this;
}

Text& Text::SetOffset(Point offset) {
    _offset = std::move(offset);
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    _font_size = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    _font_family = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    _font_weight = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string data) {
    std::stringstream res;
    for (const char c : data) {
        switch (c)
        {
        case '\"':
            res << "&quot;"sv;
            break;
        case '<':
            res << "&lt;"sv;
            break;
        case '>':
            res << "&gt;"sv;
            break;
        case '\'':
            res << " &apos;"sv;
            break;
        case '&':
            res << "&amp;"sv;
            break;        
        default:
            res << c;
            break;
        }
    }
    _data = std::move(res.str());
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv
        << " x=\""sv << _pos.x << "\""sv
        << " y=\""sv << _pos.y << "\""sv
        << " dx=\""sv << _offset.x << "\""sv
        << " dy=\""sv << _offset.y << "\""sv
        << " font-size=\""sv << _font_size << "\""sv;
        if (!_font_family.empty()) out << " font-family=\""sv << _font_family << "\""sv;
        if (!_font_weight.empty()) out << " font-weight=\""sv << _font_weight << "\""sv;        
        out << ">"sv << _data << "</text>";
}

}  // namespace svg