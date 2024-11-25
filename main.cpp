#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

using namespace std::literals;
using namespace svg;

namespace shapes {

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1), p2_(p2), p3_(p3) {}

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline{}
                          .AddPoint(p1_)
                          .AddPoint(p2_)
                          .AddPoint(p3_)
                          .AddPoint(p1_)
                          .SetFillColor("red")
                          .SetStrokeColor("black"));
    }

private:
    svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays)
        : center_(center),
          outer_rad_(outer_rad),
          inner_rad_(inner_rad),
          num_rays_(num_rays) {}

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar(center_, outer_rad_, inner_rad_, num_rays_)
                          .SetFillColor("yellow")
                          .SetStrokeColor("black"));
    }

private:
    svg::Point center_;
    double outer_rad_;
    double inner_rad_;
    int num_rays_;

    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) const {
        using namespace svg;
        Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * i / num_rays;
            polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
            angle += M_PI / num_rays;
            polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
        }
        return polyline;
    }
};

class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point head_center, double radius)
        : head_center_(head_center), radius_(radius) {}

    void Draw(svg::ObjectContainer& container) const override {
        using namespace svg;
        container.Add(Circle{}
                          .SetCenter({head_center_.x, head_center_.y})
                          .SetRadius(radius_)
                          .SetFillColor("white")
                          .SetStrokeColor("black"));
        container.Add(Circle{}
                          .SetCenter({head_center_.x, head_center_.y + radius_ * 2})
                          .SetRadius(radius_ * 1.5)
                          .SetFillColor("white")
                          .SetStrokeColor("black"));
        container.Add(Circle{}
                          .SetCenter({head_center_.x, head_center_.y + radius_ * 4})
                          .SetRadius(radius_ * 2)
                          .SetFillColor("white")
                          .SetStrokeColor("black"));
    }

private:
    svg::Point head_center_;
    double radius_;
};

}  // namespace shapes

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
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    DrawPicture(picture, doc);

    doc.Render(cout);
}
