#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>  // Added this include

namespace svg {

using Color = std::string;

inline const Color NoneColor{"none"};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
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

// Forward declaration
class ObjectContainer;

class Object {
public:
    void Render(const RenderContext& context) const;

    ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class ObjectContainer {
public:
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    template <typename T>
    void Add(T obj) {
        AddPtr(std::make_unique<T>(std::move(obj)));
    }

    virtual ~ObjectContainer() = default;
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

    Circle(const Point& center, double radius) : center_(center), radius_(radius) {}

    Circle() : center_({0, 0}) {}

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    std::vector<Point> points_;

    void RenderObject(const RenderContext& context) const override;
};

class Text final : public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);

    Text& SetOffset(Point offset);

    Text& SetFontSize(uint32_t size);

    Text& SetFontFamily(std::string font_family);

    Text& SetFontWeight(std::string font_weight);

    Text& SetData(std::string data);

private:
    Point pos_{0, 0};
    Point offset_{0, 0};
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;

    void RenderObject(const RenderContext& context) const override;

    std::string EscapeString(const std::string& str) const;
};

class Document : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg
