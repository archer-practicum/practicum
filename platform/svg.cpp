#include "svg.h"

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
    RenderContext context(out);
    
    for (const auto &shape : _shapes) shape->Render(context);
}

// ------------Polyline-----------------

Polyline& Polyline::AddPoint(Point point) {
    _points.emplace_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
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
    _data = std::move(data);
    return *this;
}

}  // namespace svg