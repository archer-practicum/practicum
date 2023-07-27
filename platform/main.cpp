#define _USE_MATH_DEFINES
#include "svg.h"
#include <vector>
#include <cmath>

namespace shapes{

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3) : _p1(p1), _p2(p2), _p3(p3) {}

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(_p1).AddPoint(_p2).AddPoint(_p3).AddPoint(_p1));
    }

private:
    svg::Point _p1;
    svg::Point _p2;
    svg::Point _p3;
};

class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_radius, double inner_radius, int num_rais) 
    : _center(center), _outer_radius(outer_radius), _inner_radius(inner_radius), _num_rays(num_rais) {}

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar());
    }

private:
    svg::Polyline CreateStar() const {
        svg::Polyline polyline;
        for (int i = 0; i <= _num_rays; ++i) {
            double angle = 2 * M_PI * (i % _num_rays) / _num_rays;
            polyline.AddPoint({_center.x + _outer_radius * sin(angle), _center.y - _outer_radius * cos(angle)});
            if (i == _num_rays) {
                break;
            }
            angle += M_PI / _num_rays;
            polyline.AddPoint({_center.x + _inner_radius * sin(angle), _center.y - _inner_radius * cos(angle)});
        }
        return polyline.SetFillColor("red").SetStrokeColor("black");
    }

    svg::Point _center;
    double _outer_radius;
    double _inner_radius;
    int _num_rays;
};

class Snowman : public svg::Drawable {
public:
    void Draw(svg::ObjectContainer& container) const override {
        double middle_radius = _head_radius * 1.5;
        svg::Point middle_center{_head_center.x, _head_center.y + 2 * _head_radius};
        double bottom_radius = _head_radius * 2;
        svg::Point bottom_center{_head_center.x, _head_center.y + 5 * _head_radius};
        container.Add(svg::Circle().SetCenter(bottom_center).SetRadius(bottom_radius).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(svg::Circle().SetCenter(middle_center).SetRadius(middle_radius).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(svg::Circle().SetCenter(_head_center).SetRadius(_head_radius).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }

    Snowman(svg::Point head_center, double head_radius) : _head_center(head_center), _head_radius(head_radius) {}

private:
    svg::Point _head_center;
    double _head_radius;
};

} // namespace shapes

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

int main() {
    using namespace svg;
    using namespace std;

    Color none_color;
    cout << none_color << endl; // none
    
    Color purple{"purple"s};
    cout << purple << endl; // purple
    
    Color rgb = Rgb{100, 200, 255};
    cout << rgb << endl; // rgb(100,200,255)

    Color rgba = Rgba{100, 200, 255, 0.5};
    cout << rgba << endl; // rgba(100,200,255,0.5)

    Circle c;
    c.SetRadius(3.5).SetCenter({1.0, 2.0});
    c.SetFillColor(rgba);
    c.SetStrokeColor(purple);

    Rgb c1(1u, 2u, 3u);
    
    Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);
}

