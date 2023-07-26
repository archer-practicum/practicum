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
        return polyline;
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
        container.Add(svg::Circle().SetCenter(bottom_center).SetRadius(bottom_radius));
        container.Add(svg::Circle().SetCenter(middle_center).SetRadius(middle_radius));
        container.Add(svg::Circle().SetCenter(_head_center).SetRadius(_head_radius));
    }

    Snowman(svg::Point head_center, double head_radius) : _head_center(head_center), _head_radius(head_radius) {}

private:
    svg::Point _head_center;
    double _head_radius;
};

}

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
    using namespace shapes;
    using namespace std;

    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    // Так как документ реализует интерфейс ObjectContainer,
    // его можно передать в DrawPicture в качестве цели для рисования
    DrawPicture(picture, doc);

    // Выводим полученный документ в stdout
    doc.Render(cout);
}