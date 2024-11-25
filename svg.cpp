#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" ";
    out << "r=\"" << radius_ << "\"";
    RenderAttrs(out);
    out << "/>";
}

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\"";
    for (size_t i = 0; i < points_.size(); ++i) {
        if (i > 0) {
            out << " ";
        }
        out << points_[i].x << "," << points_[i].y;
    }
    out << "\"";
    RenderAttrs(out);
    out << "/>";
}

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

std::string Text::EscapeString(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '&':
                escaped += "&amp;";
                break;
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            case '"':
                escaped += "&quot;";
                break;
            case '\'':
                escaped += "&apos;";
                break;
            default:
                escaped += c;
        }
    }
    return escaped;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text";
    RenderAttrs(out);
    out << " x=\"" << pos_.x << "\" y=\"" << pos_.y << "\"";
    out << " dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\"";
    out << " font-size=\"" << size_ << "\"";
    if (!font_family_.empty()) {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    out << ">" << EscapeString(data_) << "</text>";
}

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
    for (const auto& obj : objects_) {
        obj->Render(RenderContext(out, 2, 2));
    }
    out << "</svg>" << std::endl;
}

}  // namespace svg
