#include "svg.h"

namespace svg {


    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& stroke_cap) {
        switch (stroke_cap)
        {
        case StrokeLineCap::BUTT: out << "butt";
            break;
        case StrokeLineCap::ROUND: out << "round";
            break;
        case StrokeLineCap::SQUARE: out << "square";
            break;
        }
        return out;
    }
    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& stroke_join) {
        switch (stroke_join)
        {
        case StrokeLineJoin::ARCS: out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:out << "bevel";
            break;
        case StrokeLineJoin::MITER:out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:out << "round";
            break;
        }
        return out;
    }

    std::string colorPrinter::operator()(std::monostate) const {
        return "none"s;
    }
    std::string colorPrinter::operator()(Rgb color) const {
        std::stringstream ss;
        ss<<("rgb(");
        ss << (std::to_string(color.red));
        ss << (",");
        ss << (std::to_string(color.green));
        ss << (",");
        ss << (std::to_string(color.blue));
        ss << (")");
        return ss.str();
    }
    std::string colorPrinter::operator()(Rgba color) const {
        std::stringstream ss;
        ss<<("rgba(");
        ss<<(std::to_string(color.red));
        ss<<(",");
        ss<<(std::to_string(color.green));
        ss<<(",");
        ss<<(std::to_string(color.blue));
        ss<<(",");
        ss << color.opacity;
        ss<<")";
        return ss.str();
    }
    std::string colorPrinter::operator()(std::string color) const {
        return color;
    }

    std::ostream& operator<<(std::ostream& out, const Color& color) {
        out << std::visit(colorPrinter{}, color);
        return out;
    }

    // ---------- Object ------------------

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

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
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttributes(context.out);
        out << "/>"sv;
    }


    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    //<polyline points="0,100 50,25 50,75 100,0" >
    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool first = true;
        for (Point p : points_) {
            if (!first) { out << " "; }
            else { first = false; }
            out << p.x << "," << p.y;
        }
        out << "\""sv;
        RenderAttributes(context.out);
        out << "/>"sv;
    }



    // ---------- Text ------------------
    // 

    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = EncodeData(data);
        return *this;
    }

    std::string Text::EncodeData(std::string str) {
        std::string out;
        for (char c : str) {
            switch (c)
            {
            case('"'):
                out.append("&quot;");
                break;
            case('\''):
                out.append("&apos;");
                break;
            case('<'):
                out.append("&lt;");
                break;
            case('>'):
                out.append("&gt;");
                break;
            case('&'):
                out.append("&amp;");
                break;
            default:
                out.push_back(c);
                break;
            }
        }
        return out;
    }


    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;

        out << "<text";
        RenderAttributes(context.out);
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << font_size_ << "\" "sv;
        if (!font_family_.empty()) {
            out << "font-family=\""sv << font_family_ << "\" "sv;
        }
        if (!font_weight_.empty()) {
            out << "font-weight=\""sv << font_weight_ << "\"";
        }
        out << ">"sv;
        out << data_ << "</text>"sv;
    }


    // ---------- Document ------------------
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        RenderContext ctx(out, 2, 2);

        for (auto& obj : objects_) {
            obj->Render(ctx);
        }

        out << "</svg>"sv;

    }

    // ---------- Drawable------------------

    // ---------- ObjectContainer ------------------

}  // namespace svg